#ifndef _SPRITE_H_

#define _SPRITE_H_

typedef struct {
	double x, y; // current position
	int width, height;  // dimensions
	int xspeed, yspeed; // current speed
	char * map;          // the pixmap
} Sprite;

/**

 * @brief Sprite constructor
 *

 * @param x,y  coordinates of the Sprite

 *

 *@param xpeed,yspeed  horizontal and vertical speed of the sprite
 *

 * @param pic xpm that is going to be used to obtain the sprite

 * @return Sprite constructed

 */
Sprite * create_sprite(char * pic[], double x, double y, int xspeed, int yspeed);
/**

 * @brief Sprite destructor
 *

 * @param sp  Sprite that is going to be destructed

 */
void destroy_sprite(Sprite * sp);
/**

 * @brief Animates a sprite for each timer tick (1/60) s
 *

 * @param sp  Sprite that is going to be animated

 *@ return 0 in case of success, 1 otherwise

 */

int animate_sprite(Sprite * sp);
/**

 * @brief Draws a sprite on the screen
 *

 * @param sp  Sprite that is going to be drawn

 *@ return 0 in case of success, 1 otherwise

 */

int draw_sprite(Sprite * sp);

#endif
