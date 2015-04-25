#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "mouse.h"

#define DELAY_US    20000

static int mouseHookId;

int mouse_subscribe_int()
{
	int bitmask;

	mouseHookId = IRQ_MOUSE;
	bitmask = BIT(mouseHookId);

	if(sys_irqsetpolicy(IRQ_MOUSE, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouseHookId) != OK)
	{
		printf("\t mouse_subscribe_int: sys_irqsetpolicy() failed. \n");
		return -1;
	}

	if(sys_irqenable(&mouseHookId) != OK)
	{
		printf("\t mouse_subscribe_int: sys_irqenable() failed. \n");
		return -1;
	}

	return bitmask;
}
int mouse_unsubscribe_int()
{
	if(sys_irqdisable(&mouseHookId) != OK)
	{
		printf("\t mouse_unsubscribe_int: sys_disable() failed. \n");
		return -1;
	}

	if(sys_irqrmpolicy(&mouseHookId) != OK)
	{
		printf("\t mouse_unsubscribe_int: sys_irqrmpolicy() failed. \n");
		return -1;
	}

	return 0;
}

int mouse_command(unsigned long cmd)
{
	unsigned long ackB;

	sys_outb(KBC_CMD_PORT, KBC_CMD_MOUSE);

	do
	{
		sys_outb(KBC_OUT_BUF, cmd);

		tickdelay(micros_to_ticks(DELAY_US));

		sys_inb(KBC_OUT_BUF, &ackB);
		if(ackB == KBC_ACKB_ERROR)
			return KBC_ACKB_ERROR;

	} while(ackB != KBC_ACKB_ACK);

	return 0;
}

int mouse_read_packet(unsigned char* packet, unsigned short* count)
{
	unsigned long data;

	if(sys_inb(KBC_OUT_BUF, &data) != OK)
	{
		printf("\t mouse_read_byte: sys_inb() failed. \n");
		return -1;
	}

	packet[(*count)] = data & 0xFF;

	//If Bit 3 of byte 1 is not set, then the mouse is not synch
	if((packet[(*count)] & KBC_MOUSE_IS_SYNC) == 0 && (*count) == 0)
		return 1;

	(*count)++;

	if((*count) >= 3)
		(*count) = 0;

	return 0;
}
int mouse_handle_int(unsigned char* packet, unsigned short* count)
{
	int xPosition, yPosition;
	xPosition = 0;
	yPosition = 0;

	if(mouse_read_packet(packet, count) != 0)
		return -1;

	if((*count) == 0)
	{
		printf("B1=%X\t", packet[0]);
		printf("B2=%X\t", packet[1]);
		printf("B3=%X\t", packet[2]);

		printf("LB=%d\t", packet[0] & MDP_LB);
		printf("MB=%d\t", (packet[0] & MDP_MB) >> 2);
		printf("RB=%d\t", (packet[0] & MDP_RB) >> 1);
		printf("XOV=%d\t", (packet[0] & MDP_XOV) >> 6);
		printf("YOV=%d\t", (packet[0] & MDP_YOV) >> 7);

		xPosition |= packet[1] & 0xFF;
		yPosition |= packet[2] & 0xFF;

		if(packet[0] & MDP_X_SIGN)
			xPosition = xPosition | -256;
		if(packet[0] & MDP_Y_SIGN)
			yPosition = yPosition | -256;

		printf("X=%d\t", xPosition);
		printf("Y=%d\n", yPosition);
	}

	return 0;
}
int mouse_show_config()
{
	unsigned long status;

	// Requesting mouse status data
	while(mouse_command(MOUSE_STATUS_REQUEST) == KBC_ACKB_ERROR);


	// Read first byte
	if(sys_inb(KBC_OUT_BUF, &status) != OK)
	{
		printf("\t mouse.c:: mouse_show_config: sys_inb() #1 failed. \n");
		return -1;
	}

	// Byte 1
	printf("\n\n\t Byte 1: \t %X \n", status);

	// Mode
	printf("\t Mode: ");
	if(status & MOUSE_SR1_REMOTE_MODE)
		printf("\t\t Remote Mode \n");
	else
		printf("\t\t Stream Mode \n");

	// Data Reporting
	printf("\t Data Reporting: ");
	if(status & MOUSE_SR1_ENABLED)
		printf("Enabled \n");
	else
		printf("Disabled \n");

	// Scaling
	printf("\t Scaling: ");
	if(status & MOUSE_SR1_SCALING_2_1)
		printf("\t 2:1 \n");
	else
		printf("\t 1:1 \n");


	// Read second byte
	if(sys_inb(KBC_OUT_BUF, &status) != OK)
	{
		printf("\t mouse.c:: mouse_show_config: sys_inb() #2 failed. \n");
		return -1;
	}

	// Byte 2
	printf("\n\t Byte 2: \t %X \n", status);

	// Resolution
	printf("\t Resolution: \t %d \n", pow(2, status));


	// Read third byte
	if(sys_inb(KBC_OUT_BUF, &status) != OK)
	{
		printf("\t mouse.c:: mouse_show_config: sys_inb() #3 failed. \n");
		return -1;
	}

	// Byte 3
	printf("\n\t Byte 3: \t %X \n", status);

	// Sample Rate
	printf("\t Sample Rate: \t %d Hz \n\n", status);

	return 0;
}
