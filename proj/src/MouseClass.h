#ifndef _MOUSE_CLASS_H_
#define _MOUSE_CLASS_H_

#include "boolType.h"

#if(1)
#define BIT(n) 				(0x01<<(n))

#define MS_REG_OUT_BUF		0x60 	// Write args and read response
#define MS_REG_CMD			0x64 	// KBC Command Port

// Acknowledgment byte read from the out buffer
#define MS_ACKB_ACK			0xFA
#define MS_ACKB_NACK		0xFE
#define MS_ACKB_ERROR		0xFC

#define MS_CMD_MOUSE			0xD4	// KBC Mouse Command
#define MS_CMD_ENABLE 			0xF4
#define MS_CMD_DISABLE			0xF5
#define MS_CMD_STATUS_REQUEST	0xE9

#define MS_SR1_REMOTE_MODE	BIT(6)		// Else stream mode
#define MS_SR1_ENABLED		BIT(5)
#define MS_SR1_SCALING_2_1	BIT(4)		// Else scaling set to 1:1
#define MS_SR1_MOUSE_IS_SYNC	BIT(3) 	// Bit 3 of Byte 1 must be set to 1, otherwise the code is not in sync
#define MS_SR1_LB			BIT(2)
#define MS_SR1_MB			BIT(1)
#define MS_SR1_RB			BIT(0)

// Movement Data Packet BYTE 1
#define	MS_MDP_LB			BIT(0)
#define MS_MDP_RB			BIT(1)
#define MS_MDP_MB			BIT(2)
#define MS_MDP_X_SIGN		BIT(4)
#define MS_MDP_Y_SIGN		BIT(5)
#define MS_MDP_XOV			BIT(6)
#define MS_MDP_YOV			BIT(7)

#define MS_DELAY_US    		20000
#endif

typedef struct mouse_class MouseClass;
typedef enum
{
	ms_lb = 0,
	ms_mb = 1,
	ms_rb = 2
} ms_key;

MouseClass* newMouse();
void deleteMouse(MouseClass* Mouse);

bool MouseInitialize(MouseClass* Mouse);
void MouseShutdown(MouseClass* Mouse);

static bool MouseCommand(unsigned long cmd);
static bool MouseReadPacket(MouseClass* Mouse);
bool MouseHandleInt(MouseClass* Mouse);
static void MouseShowData(MouseClass* Mouse);
bool MouseShowConfig(MouseClass* Mouse);

bool MouseIsBtnDown(MouseClass* Mouse, ms_key key);
int MouseGetDeltaX(MouseClass* Mouse);
int MouseGetDeltaY(MouseClass* Mouse);
void MouseEnableMovementReport(MouseClass* Mouse, bool enable);

#endif
