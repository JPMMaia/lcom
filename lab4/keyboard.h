#include <minix/sysutil.h>

#define BIT(n) (0x01<<(n))

#define DELAY_US    20000

#define KB_IRQ		0x01
#define KB_OUT_BUF	0x60
#define KB_STAT_REG	0x64
#define KBC_CMD_REG 0x64

#define PAR_ERR		BIT(7)
#define TO_ERR		BIT(6)
#define IBF			BIT(1)
#define OBF			BIT(0)


#define BREAKCODE 	BIT(7)
#define BC_ESC	0x81

#define KBC_RCB		0x20
#define KBD_LED		0xED

#define KBE_ACK		0xFA
#define KBE_RESEND	0xFE
#define KBE_ERROR	0xFC

int kb_subscribe_int();
int kb_unsubscribe_int();
int kb_read_data(unsigned long *data);
int kb_write_data(unsigned long cmd, unsigned long arg);
int kb_handle_int();
int kb_handle_int_leds();
void kb_change_led_state();
