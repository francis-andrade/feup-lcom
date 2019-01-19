#ifndef __KBD_FUNCTIONS_H
#define __KBD_FUNCTIONS_H

/** @defgroup kbd_functions kbd_functions
 * @{
 *
 * Functions and macros that could later be reused in the project
 */

//macros
#define BIT(n) (0x01<<(n))

#define IS_BREAKCODE(c) (c>>7)

#define DELAY_US 20000

int timer_hook_id;//hook for handling timer interruptions
int kbd_hook_id;//hook for handling keyboard interruptions
//Timer functions

/**
 * @brief subscribes a timer interruption
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_subscribe_int();
/**
 * @brief unsubscribes a timer interruption
 *
 * @return Return 0 upon success and non-zero otherwise
 */

int timer_unsubscribe_int();

//Keyboard functions

/**
 * @brief subscribes a keyboard interruption
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_subscribe_int();
/**
 * @brief unsubscribes a timer interruption
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_unsubscribe_int();

/**
 * @brief reads a command from kbc
 *
 *
 * @param data variable that stores the command
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_cmd_read(unsigned long *data);
/**
 * @brief issues a command to kbc
 *
 *
 * @param cmd command to be issued
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_cmd_issue(unsigned long cmd);
/**
 * @brief issues a command to kbd
 *
 *
 * @param cmd command to be issued
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_cmd_issue(unsigned long cmd);
/**
 * @brief writes a complete command to kbd
 *this function is different from the previous one because in this case it has to write several bits to kbd
 *
 * @param n total number of commands to be written
 *
 * @param cmds commands to be issued
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_cmd_write(unsigned int n, unsigned long * cmds);






#endif
