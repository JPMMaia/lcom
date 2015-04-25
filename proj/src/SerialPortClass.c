#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

#include "SerialPortClass.h"

#define SP_DEBUG 0

struct serial_port_class
{
	unsigned short baseAddr;
	char lcr;
	char lsr;
	char ier;
	char iir;

	bool transmitter;
};

/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ====================================

SerialPortClass* newSerialPort()
{
	SerialPortClass* SerialPort;
	SerialPort = (SerialPortClass*) malloc(sizeof(SerialPortClass));

	return SerialPort;
}
void deleteSerialPort(SerialPortClass* SerialPort)
{
	free(SerialPort);
}

bool SerialPortInitialize(SerialPortClass* SerialPort, unsigned short baseAddr, bool transmitter)
{
	// Disable all interrupts
	SerialPortSetFlag(SerialPort, SP_REG_IER, SP_IER_RDI | SP_IER_TEI | SP_IER_LSI | SP_IER_MSI, false);

	// Check for errors
	char lsr;
	SerialPortReadReg(SerialPort, SP_REG_LSR, &lsr);

	if(SerialPortErrorDetection(lsr) != sp_none_error)
		return false;

	// Enable fifo
	SerialPortSetFlag(SerialPort, SP_REG_FCR, SP_FCR_EB | SP_FCR_CLEAR1 | SP_FCR_CLEAR2, true);

	SerialPort->baseAddr = baseAddr;
	SerialPort->transmitter = transmitter;

	return true;
}
void SerialPortShutdown(SerialPortClass* SerialPort)
{
	// Disable fifo
	SerialPortSetFlag(SerialPort, SP_REG_FCR, SP_FCR_EB | SP_FCR_CLEAR1 | SP_FCR_CLEAR2, false);
}


//============================= OPERATIONS ===================================

void SerialPortHandleInt(SerialPortClass* SerialPort, char data)
{
	// Read IIR
	char iir;
	SerialPortReadReg(SerialPort, SP_REG_IIR, &iir);

	if(!(iir & SP_IIR_INTP_) && (iir & SP_IIR_THRE))
		SerialPortTransmitByte(SerialPort, data);
}
bool SerialPortHandleFifo(SerialPortClass* SerialPort, char* data, unsigned long delay)
{
	// Read LSR
	char lsr;
	SerialPortReadReg(SerialPort, SP_REG_LSR, &lsr);

	unsigned long i = 0;
	while(lsr & SP_LSR_THRE)
	{
		if(lsr & SP_LSR_FIFOE)
		{
			printf("\n\t FIFO Error \n");
			return false;
		}

		SerialPortWriteReg(SerialPort, SP_REG_THR, data[i]);
		if(data[i] == '\0')
			return true;

		i++;

		SerialPortReadReg(SerialPort, SP_REG_LSR, &lsr);
	}

	return false;
}

void SerialPortTransmitData(SerialPortClass* SerialPort, char* data)
{
	// If Divisor Latch Access bit is set to 1, then we need to set it to 0
	SerialPortSetFlag(SerialPort, SP_REG_LCR, SP_LCR_DLAB, false);

	unsigned long i;
	for(i = 0; ; i++)
	{
		SerialPortTransmitByte(SerialPort, data[i]);
		if(data[i] == '\0')
			break;
	}
}
char* SerialPortReceiveData(SerialPortClass* SerialPort)
{
	char strings[256];
	char* data;

	// If Divisor Latch Access bit is set to 1, then we need to set it to 0
	SerialPortSetFlag(SerialPort, SP_REG_LCR, SP_LCR_DLAB, false);

	if(SP_DEBUG)
		printf("\n\t Receiving data \n");

	unsigned long i;
	char c;
	for(i = 0; ; i++)
	{
		if(!SerialPortReceiveByte(SerialPort, &c))
			return 0;

		if(c == '\0')
			break;

		strings[i] = c;
	}
	strings[i] = '\0';

	// Allocating memory for data
	data = (char*) malloc(sizeof(char) * i);
	strncpy(data, strings, i);

	return data;
}

void SerialPortShowConfig(SerialPortClass* SerialPort)
{
	SerialPortLCRShow(SerialPort);
	SerialPortIERShow(SerialPort);
}


//=============================   ACCESS   ===================================

unsigned short SerialPortGetBaseAddr(SerialPortClass* SerialPort)
{
	return SerialPort->baseAddr;
}


/////////////////////////////// PRIVATE //////////////////////////////////////

//============================= OPERATIONS ===================================

void SerialPortTransmitByteInt(SerialPortClass* SerialPort, char byte)
{
	SerialPortWriteReg(SerialPort, SP_REG_THR, byte);
}
void SerialPortTransmitByte(SerialPortClass* SerialPort, char byte)
{
	// Read Line Status Register
	char lsr;
	SerialPortReadReg(SerialPort, SP_REG_LSR, &lsr);
	SerialPort->lsr = lsr;

	// Waiting for transmitter ready
	while(!(lsr & SP_LSR_THRE))
	{
		tickdelay(micros_to_ticks(SP_DELAY_US));

		// Read LSR
		SerialPortReadReg(SerialPort, SP_REG_LSR, &lsr);
	}

	// Send byte
	SerialPortWriteReg(SerialPort, SP_REG_THR, byte);
}
bool SerialPortReceiveByte(SerialPortClass* SerialPort, char* byte)
{
	// Setting RDI, TEI, LSI and MSI to 0
	SerialPortSetFlag(SerialPort, SP_REG_IER, 0x0F, false);

	// Read Line Status Register
	char lsr;
	SerialPortReadReg(SerialPort, SP_REG_LSR, &lsr);

	// Error detection
	if(SerialPortErrorDetection(lsr) != sp_none_error)
		return false;

	// If Divisor Latch Access bit is set to 1, then we need to set it to 0
	SerialPortSetFlag(SerialPort, SP_REG_LCR, SP_LCR_DLAB, false);

	// Waiting for Receiver Ready bit
	while(!(lsr & SP_LSR_RD))
	{
		tickdelay(micros_to_ticks(SP_DELAY_US));

		SerialPortReadReg(SerialPort, SP_REG_LSR, &lsr);
	}

	// Read character
	SerialPortReadReg(SerialPort, SP_REG_RBR, byte);

	return true;
}

bool SerialPortSetConfig(SerialPortClass* SerialPort, char bits, char stop, long parity, unsigned long rate)
{
	char config = 0;
	if(!SerialPortSetBits(bits, &config))
		return false;

	if(!SerialPortSetStop(stop, &config))
		return false;

	SerialPortSetParity(parity, &config);

	// Writing LCR
	SerialPortWriteReg(SerialPort, SP_REG_LCR, config);

	// Setting rate
	SerialPortSetRate(SerialPort, rate);

	return 0;

}
inline bool SerialPortSetBits(char bits, char* byte)
{
	if(bits < 5 || bits > 8)
	{
		printf("\n\t SerialPortClass.c :: SerialPortSetBits : bits out of range \n");
		return false;
	}

	bits -= 5;
	*byte &= 0xFC; // bit 0 and 1 set to 0
	*byte |= bits;

	return true;
}
inline bool SerialPortSetStop(char stop, char* byte)
{
	if(stop != 1 || stop != 2)
	{
		printf("\n\t SerialPortClass.c :: SerialPortSetStop : stop out of range \n");
		return false;
	}

	if(stop == 2)
		*byte |= SP_LCR_2STOP;

	return true;
}
inline void SerialPortSetParity(sp_parity parity, char* byte)
{
	if(parity == sp_none_parity)
		return;

	*byte |= SP_LCR_NONE_;

	if(parity == sp_even_parity)
		*byte |= SP_LCR_EVEN;
}
void SerialPortSetRate(SerialPortClass* SerialPort, unsigned long rate)
{
	// Calculating divisor
	unsigned long divisor;
	divisor = SP_BIT_RATE_NUM / rate;

	// Calculating LSB and MSB
	char lsb, msb;
	lsb = divisor & 0xFF;
	msb = (divisor >> 8) & 0xFF;

	// Setting DLAB to 1
	SerialPortSetFlag(SerialPort, SP_REG_LCR, SP_LCR_DLAB, true);

	// Setting LSB
	SerialPortWriteReg(SerialPort, SP_REG_DLL, lsb);
	// Setting MSB
	SerialPortWriteReg(SerialPort, SP_REG_DLM, msb);

	// Setting DLAB to 0
	SerialPortSetFlag(SerialPort, SP_REG_LCR, SP_LCR_DLAB, false);
}

void SerialPortLCRShow(SerialPortClass* SerialPort)
{
	// Reading Line Control Register
	char reg_lcr;
	SerialPortReadReg(SerialPort, SP_REG_LCR, &reg_lcr);
	SerialPort->lcr = reg_lcr;

	// Calculating word-length
	char word_length;
	SerialPortLCRWordLength(SerialPort, &word_length);

	// Retrieving stop-bits
	unsigned long stop_bits;
	SerialPortLCRStopBits(SerialPort, &stop_bits);

	// Calculating bit-rate
	unsigned long bit_rate;
	SerialPortLCRBitRate(SerialPort, &bit_rate);

	// Calculating parity
	sp_parity parity;
	SerialPortLCRParity(SerialPort, &parity);

	// Retrieving set-break-enabled
	bool set_break_enabled;
	SerialPortLCRBreak(SerialPort, &set_break_enabled);

	// Retrieving DLAB
	bool dlab;
	SerialPortLCRDlab(SerialPort, &dlab);


	// Printing info
	printf("\n\t LCR Reg:\t\t\t0x%X", reg_lcr);

	// Printing word-length
	printf("\n\t Word Length:\t\t\t");
	printf("%d", word_length);

	// Printing number of Stop Bits
	printf("\n\t No. Stop Bits:\t\t\t");
	printf("%d", stop_bits);

	// Printing bit-rate
	printf("\n\t Bit Rate:\t\t\t");
	printf("%d Hz", bit_rate);

	// Printing parity
	printf("\n\t Parity:\t\t\t");
	if(parity == sp_none_parity)
		printf("None");
	else if(parity == sp_even_parity)
		printf("Even");
	else if(parity == sp_odd_parity)
		printf("Odd");
	else
		printf("%d", parity);

	// Priting set-break-enabled
	printf("\n\t Set Break:\t\t\t");
	if(set_break_enabled)
		printf("Enabled");
	else
		printf("Disabled");

	// Printing DLAB
	printf("\n\t DLAB:\t\t\t\t");
	if(dlab)
		printf("Selects DL");
	else
		printf("Selects Data");

	printf("\n\n");
}
inline void SerialPortLCRWordLength(SerialPortClass* SerialPort, char* wordLength)
{
	*wordLength = (SerialPort->lcr & 0x3) + 5;
}
void SerialPortLCRBitRate(SerialPortClass* SerialPort, unsigned long* bitRate)
{
	// Checking if DLAB is set and setting it if it is not
	if((SerialPort->lcr & SP_LCR_DLAB) == 0)
	{
		SerialPort->lcr |= SP_LCR_DLAB;
		SerialPortWriteReg(SerialPort, SP_REG_LCR, SerialPort->lcr);
	}

	// Read lsb and msb and calculating divisor
	char lsb;
	SerialPortReadReg(SerialPort, SP_REG_DLL, &lsb);
	char msb;
	SerialPortReadReg(SerialPort, SP_REG_DLM, &msb);

	unsigned long divisor;
	divisor = (msb << 8) | lsb;

	// Calculating bit-rate
	*bitRate = SP_BIT_RATE_NUM / divisor;

	// Resetting DLAB
	SerialPort->lcr &= ~SP_LCR_DLAB;
	SerialPortWriteReg(SerialPort, SP_REG_LCR, SerialPort->lcr);
}
inline void SerialPortLCRStopBits(SerialPortClass* SerialPort, unsigned long* stopBits)
{
	if(SerialPort->lcr & SP_LCR_2STOP)
		*stopBits = 2;
	else
		*stopBits = 1;
}
inline void SerialPortLCRParity(SerialPortClass* SerialPort, sp_parity* parity)
{
	if((SerialPort->lcr & SP_LCR_NONE_) == 0)
	{
		*parity = sp_none_parity;
		return;
	}
	if(SerialPort->lcr & SP_LCR_EVEN)
		*parity = sp_even_parity;
	else
		*parity = sp_odd_parity;
}
inline void SerialPortLCRBreak(SerialPortClass* SerialPort, bool* setBreakEnabled)
{
	if(SerialPort->lcr & SP_LCR_BREAK)
		*setBreakEnabled = true;
	else
		*setBreakEnabled = false;
}
inline void SerialPortLCRDlab(SerialPortClass* SerialPort, bool* dlab)
{
	if(SerialPort->lcr & SP_LCR_DLAB)
		*dlab = true;
	else
		*dlab = false;
}

void SerialPortIERShow(SerialPortClass* SerialPort)
{
	// Reading IER Register
	char reg_ier;
	SerialPortReadReg(SerialPort, SP_REG_IER, &reg_ier);
	SerialPort->ier = reg_ier;

	// Retrieving data
	bool rdi, tei, lsi, msi;
	SerialPortIERRdi(SerialPort, &rdi);
	SerialPortIERTei(SerialPort, &tei);
	SerialPortIERLsi(SerialPort, &lsi);
	SerialPortIERMsi(SerialPort, &msi);

	// Printing data
	printf("\n\t IER Reg:\t\t\t0x%X", reg_ier);

	printf("\n\t Received Data Int:\t\t");
	if(rdi)
		printf("Enabled");
	else
		printf("Disabled");

	printf("\n\t Transmitter Empty Int:\t\t");
	if(tei)
		printf("Enabled");
	else
		printf("Disabled");

	printf("\n\t Receiver Line Status Int:\t");
	if(lsi)
		printf("Enabled");
	else
		printf("Disabled");

	printf("\n\t Modem Status Int:\t\t");
	if(msi)
		printf("Enabled");
	else
		printf("Disabled");

	printf("\n\n");
}
inline void SerialPortIERRdi(SerialPortClass* SerialPort, bool* rdi)
{
	if(SerialPort->ier & SP_IER_RDI)
		*rdi = true;
	else
		*rdi = false;
}
inline void SerialPortIERTei(SerialPortClass* SerialPort, bool* tei)
{
	if(SerialPort->ier & SP_IER_TEI)
		*tei = true;
	else
		*tei = false;
}
inline void SerialPortIERLsi(SerialPortClass* SerialPort, bool* lsi)
{
	if(SerialPort->ier & SP_IER_LSI)
		*lsi = true;
	else
		*lsi = false;
}
inline void SerialPortIERMsi(SerialPortClass* SerialPort, bool* msi)
{
	if(SerialPort->ier & SP_IER_MSI)
		*msi = true;
	else
		*msi = false;
}

void SerialPortSetFlag(SerialPortClass* SerialPort, unsigned short spReg, char flag, bool enable)
{
	char reg;
	SerialPortReadReg(SerialPort, spReg, &reg);

	if(enable)
		reg |= flag;
	else
		reg &= ~flag;

	SerialPortWriteReg(SerialPort, spReg, reg);
}
sp_error SerialPortErrorDetection(unsigned short reg_lsr)
{
	sp_error error;

	if(reg_lsr & SP_LSR_OE)
		error = sp_overrun_error;
	else if(reg_lsr & SP_LSR_PE)
		error = sp_parity_error;
	else if(reg_lsr & SP_LSR_FE)
		error = sp_framing_error;
	else
		error = sp_none_error;

	if(error != sp_none_error)
		printf("\n\t SerialPortClass.c :: SerialPortErrorDetection : Error Detected: %d\n", error);

	return error;
}

static inline
void SerialPortReadReg(SerialPortClass* SerialPort, unsigned short spReg, char* config)
{
	char byte;
	unsigned short port = SerialPort->baseAddr + spReg;
	asm volatile ("inb %1, %0"
			: "=a" (byte)
			  : "Nd" (port)
	);

	*config = byte;
}
static inline
void SerialPortWriteReg(SerialPortClass* SerialPort, unsigned short spReg, char config)
{
	unsigned short port = SerialPort->baseAddr + spReg;
	asm volatile ("outb %0, %1"
			:
			: "a" (config), "Nd" (port)
	);
}
