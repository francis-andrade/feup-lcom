#ifndef __KBC_H
#define __KBC_H

//Keyboard functions

/** @defgroup kbc kbc
 * @{
 * Functions related to the kbc
 */

/**
 * @brief subscribes a keyboard interruption
 * @param kbd_hook_id
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_subscribe_int(unsigned int * kbd_hook_id);

/**
 * @brief unsubscribes a keyboard interruption
 * @param kbd_hook_id
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_unsubscribe_int(unsigned int * kbd_hook_id);

/**
 * @brief reads a command from kbc
 * @param data variable that stores the command
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_cmd_read(unsigned long *data);

/**
 * @brief issues a command to kbc
 * @param cmd command to be issued
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_cmd_issue(unsigned long cmd);

/**
 * @brief issues a command to kbd
 * @param cmd command to be issued
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_cmd_issue(unsigned long cmd);

/**
 * @brief writes a complete command to kbd
 * this function is different from the previous one because in this case it has to write several bits to kbd
 * @param n total number of commands to be written
 * @param cmds commands to be issued
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_cmd_write(unsigned int n, unsigned long * cmds);

/**
 * @brief converts a variable passed buy reference to a pointer that can be interpreted in assembly
 * @param scancode scancode passed by reference
 * @return result of kbc_read_ass
 */
int kbc_c_to_ass(unsigned long * scancode);
/** @} end of kbc */
#endif
