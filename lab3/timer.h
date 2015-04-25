#ifndef __TIMER_H
#define __TIMER_H

/** @defgroup timer timer
 * @{
 *
 * Functions for using the i8254 timers
 */

/**
 * @brief Configures a timer to generate a square wave
 * 
 * Does not change the LSB (BCD/binary) of the timer's control word.
 * 
 * @param timer Timer to configure. (Ranges from 0 to 2)
 * @param freq Frequency of the square wave to generate
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_set_square(unsigned long timer, unsigned long freq);

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on 
failure
 */
int timer_subscribe_int(void );

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_unsubscribe_int();

/**
 * @brief Timer 0 interrupt handler
 *
 * Increments counter
 */
void timer_int_handler();

/**
 * @brief Reads the input timer configuration
 *
 * param timer Timer whose config to read (Ranges from 0 to 2)
 * param st    Address of memory position to be filled with the timer 
config
 * return Return 0 upon success and non-zero otherwise
 */
int timer_get_config(unsigned long timer, unsigned char *st);

/**
 * @brief Shows timer configuration
 * 
 * Displays in a human friendly way, the configuration of the input
 *  timer, by providing the values (and meanings) of the different
 *  components of a timer configuration 
 *
 * param timer Timer whose config to read (Ranges from 0 to 2)
 * return Return 0 upon success and non-zero otherwise
 */
int timer_show_config(unsigned long timer);



/**
 * @brief Tests programming timer in square wave mode
 *
 * Programs Timer 0 to generate square mode with input frequency
 *
 * @param freq Frequency of square wave to generate
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_test_square(unsigned long freq);

/**
 * @brief Tests Timer 0 interrupt handling
 *
 * Subscribes Timer 0 interrupts and prints a message once
 *  per second for the specified time interval
 *
 * @param time Length of time interval while interrupts are subscribed
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_test_int(unsigned long time);

/**
 * @brief Tests display of timer config
 *
 * Just calls timer_show_config()
 *
 * param timer Timer whose config to read (Ranges from 0 to 2)
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_test_config(unsigned long timer);

#endif /* __TIMER_H */
