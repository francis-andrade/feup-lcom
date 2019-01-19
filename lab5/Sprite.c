#include "Sprite.h"
#include "utilities.h"
#include <stdlib.h>



Sprite * create_sprite(char * pic[], double x, double y, int xspeed, int yspeed) {
//allocate space for the "object"
	Sprite * sp = (Sprite * ) malloc ( sizeof(Sprite));
	if( sp == NULL )
		return NULL;
	// read the sprite pixmap
	sp->map = read_xpm(pic, &(sp->width), &(sp->height));
	if( sp->map == NULL ) {
		free(sp);
		return NULL;
	}
	sp->x=x;
	sp->y=y;
	sp->xspeed=xspeed;
	sp->yspeed=yspeed;
	return sp;
}

void destroy_sprite(Sprite * sp) {
	if( sp == NULL )
		return;
	free(sp->map);
	free(sp);
}

int animate_sprite(Sprite * sp) {
	double old_x=sp->x;
	double old_y=sp->y;
	sp->x=sp->x+0.016666666*((double) sp->xspeed);
	sp->y=sp->y+0.016666666*((double) sp->yspeed);
	if(round(sp->x) != round(old_x) || round(sp->y) != round(old_y)){
		return draw_sprite(sp);
	}
	else{
		return 0;
	}

}

int draw_sprite(Sprite * sp){
	//vg_set_screen(0);
	vg_draw_pixmap(round(sp->x),round(sp->y),sp->width, sp->height, sp->map);
}
