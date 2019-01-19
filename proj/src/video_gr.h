#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H


/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief gets pointer to variable mouse_buffer
 * @return pointer to variable mouse_buffer
 */
unsigned getMouse_buffer();

/**
 * @brief gets pointer to variable double_buffer
 * @return pointer to variable double_buffer
 */
unsigned getDouble_buffer();

/**
 * @brief gets pointer to variable video_mem
 * @return pointer to variable video_mem
 */
unsigned getVideo__mem();

/**
 * @brief gets horizontal resolution of the screen
 * @return horizontal resolution
 */
unsigned getH_res();

/**
 * @brief gets vertical resolution of the screen
 * @return vertical resolution
 */
unsigned getV_res();

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *vg_init(unsigned short mode);

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

/**
 * @brief sets the color of a pixel in the double buffer and in the mouse buffer
 * @param x,y coordinates of the pixel we want to set
 * @param color color which we want to set the pixel
 * @return 0 in case of success, 1 otherwise
 */
int vg_set_pixel(int x, int y, unsigned int color);

/**
 * @brief sets the color of a pixel in the mouse buffer
 * @param x,y coordinates of the pixel we want to set
 * @param color color which we want to set the pixel
 * @return 0 in case of success, 1 otherwise
 */
int vg_set_mouse_pixel(int x, int y, unsigned int color);

/**
 * @brief draws a square in the double buffer and in the mouse buffer
 * @param xi,yi coordinates of the superior left vertice of the square
 * @param size size of the square
 * @param color color of the square
 * @return 0 in case of success, 1 otherwise
 */
int vg_draw_square(int xi, int yi, int size,int color);

/**
 * @brief draws a circle in the double buffer and in the mouse buffer
 * @param xi,yi coordinates of the center of the circle
 * @param radius radius of the circle
 * @param color color of the circle
 * @return 0 in case of success, 1 otherwise
 */
int vg_draw_circle(int xi, int yi,int radius, int color);

/**
 * @brief copies the double_buffer into the video_buffer
 * @return 0 in case of success, 1 otherwise
 */
int swap_double_video();

/**
 * @brief copies the mouse_buffer into the video_buffer
 * @return 0 in case of success, 1 otherwise
 */
int swap_mouse_video();

/**
 * @brief copies the mouse_buffer into the double_buffer
 * @return 0 in case of success, 1 otherwise
 */
int swap_mouse_double();

/**
 * @brief copies the double_buffer into the mouse_buffer
 * @return 0 in case of success, 1 otherwise
 */
int swap_double_mouse();

/**
 * @brief sets a pixel in the mouse buffer to the color of the corresponding pixel in the double buffer
 * @param x,y coordinates of the pixel
 * @return 0 in case of success, 1 otherwise
 */
int mouse_to_double(int x, int y);
 /** @} end of video_gr */
 
#endif /* __VIDEO_GR_H */
