int sp_error_detection(unsigned short reg_lsr);
int sp_lcr_set_dlab(unsigned short base_addr, unsigned long enable);

int sp_show_config(unsigned short base_addr);

int sp_lcr_show(unsigned short base_addr);
void sp_lcr_word_length(unsigned long reg_lcr, unsigned long* word_length);
int sp_lcr_bit_rate(unsigned short base_addr, unsigned long reg_lcr, unsigned long* bit_rate);
void sp_lcr_stop_bits(unsigned long reg_lcr, unsigned long* stop_bits);

/*
 * Parity will be equals to:
 * 	-1, if none
 * 	0, if 0
 * 	1, if 1
 * 	2, if even
 * 	3, if odd
 *
 * 	@param reg_lcr the LCR register
 * 	@param parity a pointer to the returned value
 */
void sp_lcr_parity(unsigned long reg_lcr, long* parity);
void sp_lcr_break(unsigned long reg_lcr, unsigned long* set_break_enabled);
void sp_lcr_dlab(unsigned long reg_lcr, unsigned long* dlab);

int sp_ier_show(unsigned short base_addr);
void sp_ier_rdi(unsigned long reg_ier, unsigned long* rdi_enabled);
void sp_ier_tei(unsigned long reg_ier, unsigned long* tei_enabled);
void sp_ier_lsi(unsigned long reg_ier, unsigned long* lsi_enabled);
void sp_ier_msi(unsigned long reg_ier, unsigned long* msi_enabled);

int sp_set_config(unsigned short base_addr, unsigned long bits, unsigned long stop, long parity, unsigned long rate);
int sp_set_bits(unsigned long bits, unsigned long* byte);
int sp_set_stop(unsigned long stop, unsigned long* byte);
int sp_set_parity(long parity, unsigned long* byte);
int sp_set_rate(unsigned short base_addr, unsigned long rate);

int sp_transmit(unsigned short base_addr, unsigned long data);
int sp_transmit_data(unsigned short base_addr, int stringc, char* strings[]);
int sp_transmit_word(unsigned short base_addr, char* word, unsigned long last);

int sp_receive(unsigned short base_addr, unsigned long* data);
char* sp_receive_data(unsigned short base_addr);

int sp_enable_int(unsigned short base_addr, unsigned long bitmask);
int sp_int(unsigned short base_addr, int stringc, char* strings[]);
int sp_int_handler(unsigned short base_addr, unsigned long data);
int sp_transmit_int(unsigned short base_addr, unsigned long data);

int sp_subscribe(unsigned short base_addr);
int sp_unsubscribe(unsigned short base_addr);

int sp_fifo(unsigned short base_addr, int stringc, char* strings[], unsigned long delay);
int sp_enable_fifo(unsigned short base_addr, unsigned long bitmask);
int sp_fifo_handler(unsigned short base_addr, unsigned long* data, unsigned long delay);

