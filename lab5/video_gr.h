#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H




/**

 * @brief gets horizontal resolution of the screen
 *

 * @return horizontal resolution

 */
unsigned getH_res();
/**

 * @brief gets vertical resolution of the screen
 *

 * @return vertical resolution

 */
unsigned getV_res();

/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

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
 * 
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);
/**

 * @brief sets the color of a pixel
 *

 *

 * @param x,y  coordinates of the pixel we want to set

 *

 * @param color color which we want to set the pixel

 * @return 0 in case of success, 1 otherwise

 */
int vg_set_pixel(long x, long y, long color);
/**

 * @brief draws a line on the screen
 *

 *

 * @param xi,yi  coordinates of one extreme point of the line

 *

 *@param xf,yf  coordinates of one extreme point of the line

 *

 * @param color color which we want to set the line

 * @return 0 in case of success, 1 otherwise

 */
void vg_draw_line(int x1, int y1, int x2, int y2, int color);
/**

 * @brief paints the screen with one color
 *

 *

 * @param xi,yi  coordinates of the left top corner of the pixmap

 *

 *@param h_size,v_size  horizontal and vertical dimensions of the pixmap

 *

 * @param pixmap pixmap we want to draw

 * @return 0 in case of success, 1 otherwise

 */
int vg_draw_pixmap(int xi, int yi, int h_size, int v_size, char * pixmap);
/**

 * @brief paints the screen with one color
 *

 * @param color color which we want to paint the screen

 * @return 0 in case of success, 1 otherwise

 */
int vg_set_screen(unsigned long color);

 /** @} end of video_gr */
 
#endif /* __VIDEO_GR_H */
