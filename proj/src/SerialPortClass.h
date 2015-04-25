#ifndef _SERIAL_PORT_CLASS_H_
#define _SERIAL_PORT_CLASS_H_

#include "boolType.h"

#if(1)
#define BIT(n) 			(0x01<<(n))
#define SP_DELAY_US    	5000 // tickdelay(micros_to_ticks(DELAY_US));

#define SP_IRQ3			3
#define SP_IRQ4			4
#define SP_COM1			0x3F8
#define SP_COM2			0x2F8

#define SP_BIT_RATE_NUM	115200

#define	SP_REG_RBR		0	// For reading 	// SP_LCR_DLAB must be 0
#define	SP_REG_THR		0	// For writing	// SP_LCR_DLAB must be 0
#define	SP_REG_IER		1	// Interrupt Enable Register
#define	SP_REG_IIR		2	// Interrupt Identification Register
#define SP_REG_FCR		2	// Write to FIFO Control Register
#define	SP_REG_LCR		3	// Line Control Register
#define	SP_REG_MCR		4
#define	SP_REG_LSR		5	// Line Status Register
#define	SP_REG_MSR		6
#define	SP_REG_SR		7
#define SP_REG_DLL		0	// SP_LCR_DLAB must be 1
#define SP_REG_DLM		1	// SP_LCR_DLAB must be 1

#define SP_LCR_6PC		BIT(0)
#define SP_LCR_7PC		BIT(1)
#define SP_LCR_8PC		BIT(0) | BIT(1)
#define SP_LCR_2STOP	BIT(2)
#define SP_LCR_NONE_	BIT(3)
#define SP_LCR_EVEN		BIT(4)
#define SP_LCR_0_OR_1	BIT(5)
#define	SP_LCR_BREAK	BIT(6)
#define	SP_LCR_DLAB		BIT(7)		// Divisor Latch Access (overload with bit 0 and 1)

// Bits 0 to 4 are reset when LSR is read
// When using FIFO buffering the operation of these bits
//may be slightly different
#define	SP_LSR_RD		BIT(0)
#define	SP_LSR_OE		BIT(1)
#define SP_LSR_PE		BIT(2)
#define	SP_LSR_FE		BIT(3)
#define	SP_LSR_BI		BIT(4)
#define	SP_LSR_THRE		BIT(5)
#define	SP_LSR_TER		BIT(6)
#define	SP_LSR_FIFOE	BIT(7)

#define SP_IER_NONE		0
#define SP_IER_RDI		BIT(0)
#define SP_IER_TEI		BIT(1)
#define SP_IER_LSI		BIT(2)
#define SP_IER_MSI		BIT(3)

#define SP_IIR_INTP_	BIT(0)
#define SP_IIR_RLS		BIT(1) | BIT(2)
#define SP_IIR_RDA		BIT(2)
#define SP_IIR_CT		BIT(3)
#define SP_IIR_THRE		BIT(1)
#define SP_IIR_ID		BIT(1) | BIT(2) | BIT(3)

#define SP_FCR_EB		BIT(0)
#define SP_FCR_CLEAR1	BIT(1)
#define SP_FCR_CLEAR2	BIT(2)
#endif

typedef struct serial_port_class SerialPortClass;
typedef enum
{
	sp_none_error = 0,
	sp_overrun_error = 1,
	sp_parity_error = 2,
	sp_framing_error = 3,
} sp_error;
typedef enum
{
	sp_none_parity = 0,
	sp_odd_parity = 1,
	sp_even_parity = 2
} sp_parity;


SerialPortClass* newSerialPort();
void deleteSerialPort(SerialPortClass* SerialPort);

bool SerialPortInitialize(SerialPortClass* SerialPort, unsigned short baseAddr, bool transmitter);
void SerialPortShutdown(SerialPortClass* SerialPort);

void SerialPortHandleInt(SerialPortClass* SerialPort, char data);
bool SerialPortHandleFifo(SerialPortClass* SerialPort, char* data, unsigned long delay);

void SerialPortTransmitData(SerialPortClass* SerialPort, char* data);
char* SerialPortReceiveData(SerialPortClass* SerialPort);

void SerialPortShowConfig(SerialPortClass* SerialPort);

static void SerialPortTransmitByteInt(SerialPortClass* SerialPort, char byte);
static void SerialPortTransmitByte(SerialPortClass* SerialPort, char byte);
static bool SerialPortReceiveByte(SerialPortClass* SerialPort, char* byte);

static bool SerialPortSetConfig(SerialPortClass* SerialPort, char bits, char stop, long parity, unsigned long rate);
static inline bool SerialPortSetBits(char bits, char* byte);
static inline bool SerialPortSetStop(char stop, char* byte);
static inline void SerialPortSetParity(sp_parity parity, char* byte);
static void SerialPortSetRate(SerialPortClass* SerialPort, unsigned long rate);

static void SerialPortLCRShow(SerialPortClass* SerialPort);
static inline void SerialPortLCRWordLength(SerialPortClass* SerialPort, char* wordLength);
static void SerialPortLCRBitRate(SerialPortClass* SerialPort, unsigned long* bitRate);
static inline void SerialPortLCRStopBits(SerialPortClass* SerialPort, unsigned long* stopBits);
static inline void SerialPortLCRParity(SerialPortClass* SerialPort, sp_parity* parity);
static inline void SerialPortLCRBreak(SerialPortClass* SerialPort, bool* setBreakEnabled);
static inline void SerialPortLCRDlab(SerialPortClass* SerialPort, bool* dlab);

static void SerialPortIERShow(SerialPortClass* SerialPort);
static inline void SerialPortIERRdi(SerialPortClass* SerialPort, bool* rdi);
static inline void SerialPortIERTei(SerialPortClass* SerialPort, bool* tei);
static inline void SerialPortIERLsi(SerialPortClass* SerialPort, bool* lsi);
static inline void SerialPortIERMsi(SerialPortClass* SerialPort, bool* msi);

static void SerialPortSetFlag(SerialPortClass* SerialPort, unsigned short spReg, char flag, bool enable);
static sp_error SerialPortErrorDetection(unsigned short reg_lsr);

static inline void SerialPortReadReg(SerialPortClass* SerialPort, unsigned short spReg, char* config);
static inline void SerialPortWriteReg(SerialPortClass* SerialPort, unsigned short spReg, char config);

#endif
