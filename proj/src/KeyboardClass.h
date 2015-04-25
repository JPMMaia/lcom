#ifndef _KEYBOARD_CLASS_H_
#define _KEYBOARD_CLASS_H_

#include "boolType.h"

#if(1)
#define BIT(n) (0x01<<(n))

#define KB_DELAY_US    		20000

#define KB_REG_IN_BUF		0x60
#define KB_REG_OUT_BUF		0x60
#define KB_REG_STATUS		0x64
#define KBCREG_CMD 			0x64

#define KB_ST_PARITY_ERR	BIT(7)
#define KB_ST_TIME_OUT_ERR	BIT(6)
#define KB_ST_IN_BUF_FULL	BIT(1)
#define KB_ST_OUT_BUF_FULL	BIT(0)

#define KB_BREAKCODE 		BIT(7)
#define KB_BC_ESC			0x81

#define KBC_RCB				0x20
#define KBD_CMD_LED			0xED

#define KB_ACKB_ACK			0xFA
#define KB_ACKB_RESEND		0xFE
#define KB_ACKB_ERROR		0xFC
#endif

typedef struct keyboard_class KeyboardClass;
typedef enum
{
	kb_esc = 0x01,
	kb_1 = 0x02,
	kb_2 = 0x03,
	kb_3 = 0x04,
	kb_4 = 0x05,
	kb_5 = 0x06,
	kb_6 = 0x07,
	kb_7 = 0x08,
	kb_8 = 0x09,
	kb_9 = 0xA,
	kb_enter = 0x1C,
	kb_space = 0x39,
	kb_w = 0x11,
	kb_a = 0x1E,
	kb_s = 0x1F,
	kb_d = 0x20,
	kb_arrow_right = 0x4D + KB_BREAKCODE,
	kb_arrow_up = 0x48 + KB_BREAKCODE,
	kb_arrow_left = 0x4B + KB_BREAKCODE,
	kb_arrow_down = 0x50 + KB_BREAKCODE
} kb_key;

struct keyboard_class;

KeyboardClass* newKeyboard();
void deleteKeyboard(KeyboardClass* Keyboard);

bool KeyboardHandleInt(KeyboardClass* Keyboard);
void KeyboardShowData(KeyboardClass* Keyboard);

unsigned long KeyboardGetData(KeyboardClass* Keyboard);

static bool KeyboardReadData(KeyboardClass* Keyboard);
static bool KeyboardWriteData(unsigned long cmd, unsigned long* arg);

#endif
