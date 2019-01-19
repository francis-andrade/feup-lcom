#ifndef __PERIPHERALS_H
#define __PERIPHERALS_H

//macros

#define BIT(n) (0x01<<(n))

#define IS_BREAKCODE(c) (c>>7)

#define DELAY_US 20000

int timer_hook_id;//hook for handling timer interruptions

//generic functions
void ignore_interrupts(unsigned char hook_bit);

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
/**

 * @brief subscribes a mouse interruption

 *

 * @return Return 0 upon success and non-zero otherwise

 */
int mouse_subscribe_int(unsigned int * mouse_hook_id);
/**

 * @brief unsubscribes a timer interruption

 *
 *
 *@param mouse_hook_id pointer to the hook of the mouse
 *
 *

 * @return Return 0 upon success and non-zero otherwise

 */
int mouse_unsubscribe_int(unsigned int * mouse_hook_id);
/**

 * @brief writes a command to kbd

 *
 *
 *@param mouse_hook_id pointer to the hook of the mouse

 *

 * @param cmd command to be issued

 *

 * @return Return 0 upon success and non-zero otherwise

 */

int mouse_cmd_write(unsigned  long cmd);
/**

 * @brief reads a command from mouse

 *

 *

 * @param data variable that stores the command

 *

 * @return Return 0 upon success and non-zero otherwise

 */

int mouse_cmd_read(unsigned long * data);
/**

 * @brief gets a packet from mouse

 *

 *

 * @param next_byte next_byte of the packet to be read

 *
 *
 *@param packet variable that is going to store the packet

 * @return Return 0 upon success and non-zero otherwise

 */

int mouse_get_packet(unsigned long *next_byte, unsigned char * packet);



#endif
