#define BIT(n) (0x01<<(n))

#define IRQ_MOUSE			12

#define KBC_OUT_BUF			0x60	// Write args and read response
#define KBC_CMD_PORT		0x64	// KBC Command Port
#define KBC_CMD_MOUSE		0xD4	// KBC Mouse Command

// Bit 3 of Byte 1 must be set to 1, otherwise the code is not in sync
#define KBC_MOUSE_IS_SYNC	BIT(3)

// Acknowledgment byte read from the out buffer
#define KBC_ACKB_ACK		0xFA
#define KBC_ACKB_NACK		0xFE
#define KBC_ACKB_ERROR		0xFC

#define MOUSE_ENABLE 			0xF4
#define MOUSE_DISABLE			0xF5
#define MOUSE_STATUS_REQUEST	0xE9

#define MOUSE_SR1_REMOTE_MODE	BIT(6)	// Else stream mode
#define MOUSE_SR1_ENABLED		BIT(5)
#define MOUSE_SR1_SCALING_2_1	BIT(4)	// Else scaling set to 1:1
#define MOUSE_SR1_LB			BIT(2)
#define MOUSE_SR1_MB			BIT(1)
#define MOUSE_SR1_RB			BIT(0)


// Movement Data Packet BYTE 1
#define	MDP_LB				BIT(0)
#define MDP_RB				BIT(1)
#define MDP_MB				BIT(2)
#define MDP_X_SIGN			BIT(4)
#define MDP_Y_SIGN			BIT(5)
#define MDP_XOV				BIT(6)
#define MDP_YOV				BIT(7)
