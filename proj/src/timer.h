#ifndef __TIMER_H
#define __TIMER_H

/** @defgroup timer timer
 * @{
 * Functions related to the timer
 */

/**
 * @brief hook for handling timer interruptions
 */
int timer_hook_id;

//generic functions
void ignore_interrupts(unsigned char hook_id);

//Timer functions

/**
 * @brief subscribes a timer interruption
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_subscribe_int();

/**
 * @brief unsubscribes a timer interruption
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_unsubscribe_int();

/**
 * @brief sets the timer frequency
 * @param timer -> timer
 * @param freq -> frequence
 * @return 1 in case of success, o otherwise
 * */
int timer_set_square(unsigned long timer, unsigned long freq);



/**
 * @brief Reads the input timer configuration via read-back command
 * @param timer Timer whose config to read (Ranges from 0 to 2)
 * @param st    Address of memory position to be filled with the timer config
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_get_conf(unsigned long timer, unsigned char *st);

/** @} end of timer */
#endif
