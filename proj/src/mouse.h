#ifndef __MOUSE_H
#define __MOUSE_H

/** @defgroup mouse mouse
 * @{
 * Functions to manipulate the mouse
 */

/*
 * @brief This struct represents a mouse packet
 */
typedef struct{
	char lb;/**< @brief 1 if left button is pressed, 0 otherwise */
	char mb;/**< @brief 1 if middle button is pressed, 0 otherwise */
	char rb;/**< @brief 1 if right button is pressed, 0 otherwise */
	int x;/**< @brief value of the x movement */
	int y;/**< @brief value of the y movement */
}mouse_packet_t;

//MOUSE FUNCTIONS
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
 * @param data variable that stores the command
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_cmd_read(unsigned long * data);

/**
 * @brief gets a packet from mouse
 * @param next_byte next_byte of the packet to be read
 * @param packet variable that is going to store the packet
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_get_packet(unsigned long *next_byte, unsigned char * packet);

/**
 * @brief convert a packet array to mouse packet that is more user friendly
 * @param array_packet -> array packet
 * @param packet -> variable that is going to store the mouse packet
 *
 */
void mouse_convert_packet(unsigned char *array_packet, mouse_packet_t * packet);
/** @}* end of mouse */

#endif
