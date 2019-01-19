#ifndef __TEST3_H
#define __TEST3_H


/** @defgroup test3 test3
 * @{
 *
 * Functions for testing the kbd code
 */


/** 
 * @brief Displays the scancodes received from the keyboard in case the language chosen was c
 * Exits upon release of the ESC key
 *
 * @param two_bytes represents the postion of the current bit in the scancode
 *        0 if it is only one byte
 *        1 if there are two bytes and the current one is the first one
 *        2 if there are two bytes and current one is the second one
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_test_scan_c(unsigned long *scancode, int * two_bytes);



/**
 * @brief To test reading of scancode via KBD interrupts 
 * 
 * Displays the scancodes received from the keyboard
 * Exits upon release of the ESC key
 * 
 * @param ass Which IH to use: if 0, in C, otherwise in assembly
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_test_scan(unsigned short ass);

/**
 * @brief To test keyboard commands 
 * 
 * Toggles LED indicators, one per second, according to the 
 *  arguments
 * 
 * @param n number of elements of the second argument
 * @param leds array with the LEDs to toggle (Ranges from 0 t0 2)
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_test_leds(unsigned short n, unsigned short *leds);

/** 
 * @brief To test handling of more than one interrupt
 * 
 *  Similar to kbd_test_scan() except that it 
 *  should terminate also if no scancodes are received for n seconds
 * 
 * @param n Number of seconds without scancodes before exiting
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_test_timed_scan(unsigned short n);

#endif /* __TEST3_H */
