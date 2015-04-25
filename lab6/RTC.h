#define BIT(n) (0x01<<(n))

#define RTC_IRQL			8
#define RTC_ADDR_REG		0x70
#define RTC_DATA_REG		0x71

#define RTC_SEC				0
#define RTC_SECA			1
#define RTC_MIN				2
#define RTC_MINA			3
#define RTC_H				4
#define RTC_HA				5
#define RTC_DW				6
#define RTC_DM				7
#define RTC_MONTH			8
#define RTC_YEAR			9
#define RTC_REG_A			10
#define RTC_REG_B			11
#define RTC_REG_C			12
#define RTC_REG_D			13

#define RTC_A_UIP			BIT(7)
#define RTC_A_RS3			BIT(3)
#define RTC_A_RS2			BIT(2)
#define RTC_A_RS1			BIT(1)
#define RTC_A_RS0			BIT(0)

#define RTC_B_SET			BIT(7)
#define RTC_B_PIE			BIT(6)
#define RTC_B_AIE			BIT(5)
#define RTC_B_UIE			BIT(4)
#define RTC_B_SQWE			BIT(3)
#define RTC_B_DM			BIT(2)
#define RTC_B_2412			BIT(1)
#define RTC_B_DSE			BIT(0)

#define RTC_C_IRQF			BIT(7)
#define RTC_C_PF			BIT(6)
#define RTC_C_AF			BIT(5)
#define RTC_C_UF			BIT(4)

#define RTC_D_VRT			BIT(7)

int rtc_subscribe_int();
int rtc_unsubscribe_int();
int rtc_read_reg(unsigned long addr, unsigned long* config);
int rtc_write_reg(unsigned long addr, unsigned long config);
int rtc_show_config();
int rtc_show_date();
void rtc_print_time(unsigned long hour, unsigned long minute, unsigned long second, unsigned long bin);
void rtc_print_date(unsigned long dayWeek, unsigned long dayMonth, unsigned long month, unsigned long year, unsigned long bin);
int rtc_set_flag(unsigned long addr, unsigned long flag, char enable);
int rtc_set_periodic_rate(unsigned long mode);
int rtc_clear_flagsC();
