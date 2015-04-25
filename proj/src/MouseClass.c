#include "MouseClass.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

struct mouse_class
{
	unsigned char packet[3];
	unsigned short count;

	// Data
	bool keys[3];
	bool XOverflow;
	bool YOverflow;
	int deltaX;
	int deltaY;

	// Debug
	bool enableMovementReport;
};


/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ====================================

MouseClass* newMouse()
{
	unsigned long i;

	MouseClass* Mouse;
	Mouse = (MouseClass*) malloc(sizeof(MouseClass));

	for(i = 0; i < 3; i++)
		Mouse->keys[i] = false;

	return Mouse;
}
void deleteMouse(MouseClass* Mouse)
{
	free(Mouse);
}

bool MouseInitialize(MouseClass* Mouse)
{
	if(MouseCommand(MS_CMD_ENABLE) == false)
		return false;

	return true;
}
void MouseShutdown(MouseClass* Mouse)
{
	MouseCommand(MS_CMD_DISABLE);
}


//============================= OPERATIONS ===================================

bool MouseHandleInt(MouseClass* Mouse)
{
	if(MouseReadPacket(Mouse) != true)
		return false;

	if(Mouse->count == 0)
	{
		Mouse->keys[ms_lb] = (bool) Mouse->packet[0] & MS_MDP_LB;
		Mouse->keys[ms_mb] = (bool) (Mouse->packet[0] & MS_MDP_MB) >> 2;
		Mouse->keys[ms_rb] = (bool) (Mouse->packet[0] & MS_MDP_RB) >> 1;
		Mouse->XOverflow = (bool) (Mouse->packet[0] & MS_MDP_XOV) >> 6;
		Mouse->YOverflow = (bool) (Mouse->packet[0] & MS_MDP_YOV) >> 7;

		Mouse->deltaX = (int) Mouse->packet[1] & 0xFF;
		Mouse->deltaY = (int) Mouse->packet[2] & 0xFF;

		if(Mouse->packet[0] & MS_MDP_X_SIGN)
			Mouse->deltaX = Mouse->deltaX | ~0xFF;
		if(Mouse->packet[0] & MS_MDP_Y_SIGN)
			Mouse->deltaY = Mouse->deltaY | ~0xFF;

		if(Mouse->enableMovementReport)
			MouseShowData(Mouse);
	}

	return true;
}
bool MouseShowConfig(MouseClass* Mouse)
{
	unsigned long status, i;

	// Requesting mouse status data
	MouseCommand(MS_CMD_STATUS_REQUEST);

	// Read first byte

	if(sys_inb(MS_REG_OUT_BUF, &status) != OK)
	{
		printf("\t MouseClass.c :: MouseShowConfig :  sys_inb() #1 failed. \n");
		return false;
	}

	// Byte 1
	printf("\n\n\t Byte 1: \t %X \n", status);

	// Mode
	printf("\t Mode: ");
	if(status & MS_SR1_REMOTE_MODE)
		printf("\t\t Remote Mode \n");
	else
		printf("\t\t Stream Mode \n");

	// Data Reporting
	printf("\t Data Reporting: ");
	if(status & MS_SR1_ENABLED)
		printf("Enabled \n");
	else
		printf("Disabled \n");

	// Scaling
	printf("\t Scaling: ");
	if(status & MS_SR1_SCALING_2_1)
		printf("\t 2:1 \n");
	else
		printf("\t 1:1 \n");


	// Read second byte
	if(sys_inb(MS_REG_OUT_BUF, &status) != OK)
	{
		printf("\t MouseClass.c :: MouseShowConfig : sys_inb() #2 failed. \n");
		return false;
	}

	// Byte 2
	printf("\n\t Byte 2: \t %X \n", status);

	// Resolution
	printf("\t Resolution: \t %d \n", pow(2, status));


	// Read third byte
	if(sys_inb(MS_REG_OUT_BUF, &status) != OK)
	{
		printf("\t MouseClass.c :: MouseShowConfig : sys_inb() #3 failed. \n");
		return false;
	}

	// Byte 3
	printf("\n\t Byte 3: \t %X \n", status);

	// Sample Rate
	printf("\t Sample Rate: \t %d Hz \n\n", status);

	return true;
}


//=============================   ACCESS   ===================================

int MouseGetDeltaX(MouseClass* Mouse)
{
	return Mouse->deltaX;
}
int MouseGetDeltaY(MouseClass* Mouse)
{
	// Changing direction
	return -Mouse->deltaY;
}


void MouseEnableMovementReport(MouseClass* Mouse, bool enable)
{
	Mouse->enableMovementReport = enable;
}


//============================= INQUIRY    ===================================

bool MouseIsBtnDown(MouseClass* Mouse, ms_key key)
{
	return Mouse->keys[key];
}


/////////////////////////////// PRIVATE //////////////////////////////////////

//============================= OPERATIONS ===================================

static bool MouseCommand(unsigned long cmd)
{
	unsigned long ackB;

	if(sys_outb(MS_REG_CMD, MS_CMD_MOUSE) != OK)
	{
		printf("\t MouseClass.c :: MouseCommand : sys_outb() #1 failed. \n");
		return false;
	}

	do
	{
		if(sys_outb(MS_REG_OUT_BUF, cmd) != OK)
		{
			printf("\t MouseClass.c :: MouseCommand : sys_outb() #2 failed. \n");
			return false;
		}

		tickdelay(micros_to_ticks(MS_DELAY_US));

		if(sys_inb(MS_REG_OUT_BUF, &ackB) != OK)
		{
			printf("\t MouseClass.c :: MouseCommand : sys_inb() failed. \n");
			return false;
		}
		if(ackB == MS_ACKB_ERROR)
			return false;

	} while(ackB != MS_ACKB_ACK);

	return true;
}
static bool MouseReadPacket(MouseClass* Mouse)
{
	unsigned long data;

	if(sys_inb(MS_REG_OUT_BUF, &data) != OK)
	{
		printf("\t MouseClass.c :: MouseReadPacket : sys_inb() failed. \n");
		return false;
	}

	Mouse->packet[Mouse->count] = data & 0xFF;

	//If Bit 3 of byte 1 is not set, then the mouse is not synch
	if((Mouse->packet[Mouse->count] & MS_SR1_MOUSE_IS_SYNC) == 0 && Mouse->count == 0)
		return false;

	Mouse->count++;

	if(Mouse->count >= 3)
		Mouse->count = 0;

	return true;
}
static void MouseShowData(MouseClass* Mouse)
{
	printf("B1=%X\t", Mouse->packet[0]);
	printf("B2=%X\t", Mouse->packet[1]);
	printf("B3=%X\t", Mouse->packet[2]);

	printf("LB=%d\t", Mouse->keys[ms_lb]);
	printf("MB=%d\t", Mouse->keys[ms_mb]);
	printf("RB=%d\t", Mouse->keys[ms_rb]);
	printf("XOV=%d\t", Mouse->XOverflow);
	printf("YOV=%d\t", Mouse->YOverflow);

	printf("X=%d\t", Mouse->deltaX);
	printf("Y=%d\n", Mouse->deltaY);
}
