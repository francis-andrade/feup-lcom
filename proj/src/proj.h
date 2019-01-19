#ifndef __PROJ_H
#define __PROJ_H
/** @defgroup proj proj
 * @{
 * Main function and serial port connection function
 */

#include "ser_port.h"

#define PORT_ADDR SER_COM1_ADDR
#define PORT_BITS 8
#define PORT_STOP 2
#define PORT_PARITY 0
#define PORT_RATE 9600

/**
 * @brief Main Function
 */
int main(int argc, char **argv);

/**
 * @brief print_usage
 */
static void print_usage(char **argv);

/**
 * @brief print_args
 */
static int proc_args(int argc, char **argv);


/**
 * @brief ret_path_images
 */
static void ret_path_images(char * arg, char * path_images);

/**
 *@brief establishes a connection in the serial port
 *@param player player that is establishing the connection
 *@param arg0 parameter that will later be used to find the images path
 *@return 0 in case of success, 1 otherwise
 */
int connect_serial(char player, char * arg0);
/** @}* end of proj */











#endif
