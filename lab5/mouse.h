#include "KBC.h"

int mouse_subscribe_int();
int mouse_unsubscribe_int();

int mouse_command(unsigned long cmd);
int mouse_read_byte(unsigned char* packet, unsigned short* count);
int mouse_handle_int(unsigned char* packet, unsigned short* count);
int mouse_show_config();
