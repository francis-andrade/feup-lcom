#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"

#define BIT(n) (0x01<<(n))

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

/* Private global variables */

static char *video_mem; /* Process address to which VRAM is mapped */

static unsigned h_res; /* Horizontal screen resolution in pixels */
static unsigned v_res; /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */


unsigned getH_res() {
	return h_res;
}

unsigned getV_res() {
	return v_res;
}

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.b.ah = 0x00; /* Set Video Mode function */
	reg86.u.b.al = 0x03; /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}

void *vg_init(unsigned short mode) {
	struct reg86u reg;
	reg.u.b.ah = VBE_FUNCTION_INVOKE;
	reg.u.b.al = VBE_SET_MODE;
	reg.u.w.bx = mode | BIT(VBE_MODE_LINEAR_BIT);
	reg.u.b.intno = VBE_INTERRUPT;
	if (sys_int86(&reg) != OK) {
		return NULL;
	}
	else if (reg.u.b.al != VBE_FUNCTION_SUPPORTED || reg.u.b.ah != VBE_FUNCTION_SUCCESS) {
		return NULL;
	}
	else {
		vbe_mode_info_t vbe_config;
		if (vbe_get_mode_info(mode, &vbe_config) != 0) {
			return NULL;
		}
		h_res = vbe_config.XResolution;
		v_res = vbe_config.YResolution;
		bits_per_pixel = vbe_config.BitsPerPixel;
		int r;
		struct mem_range mr;
		unsigned int vram_size = h_res * v_res * bits_per_pixel;
		mr.mr_base = vbe_config.PhysBasePtr;
		mr.mr_limit = mr.mr_base + vram_size;
		if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
			panic("sys_privctl (ADD_MEM) failed: %d\n", r);
			return NULL;
		}
		video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
		if (video_mem == MAP_FAILED) {
			panic("couldn’t map video memory");
			return NULL;
		}
		else if (video_mem == NULL) {
			return NULL;
		}
		else {
			return video_mem;
		}
	}

}

int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color) {
	if (x >= h_res || y >= v_res) {//checks that the coordinates of the point are valid
		return 1;
	} else {
		*(video_mem + (x + (y * h_res)) * (bits_per_pixel / 8)) = color;
		return 0;
	}
}

void vg_draw_line(int x1, int y1, int x2, int y2, int color) {
	int d; /* Decision var
	 iable                */
	int dx, dy; /* Dx and Dy va
	 lues for the line    */
	int Eincr, NEincr; /* Decision var
	 iable increments     */
	int yincr; /* Increment fo
	 r y values           */
	int t; /* Counters etc
	 .                    */
#define ABSS(a)   ((a) >= 0 ? (a) : -(a))
	dx = ABSS(x2 - x1);
	dy = ABSS(y2 - y1);
	if (dy <= dx) {
		/* We have a line with a slope between -1 a
		 nd 1
		 *
		 * Ensure that we are always scan convertin
		 g the line from left to
		 * right to ensure that we produce the same
		 line from P1 to P0 as the
		 * line from P0 to P1.
		 */
		if (x2 < x1) {
			t = x2;
			x2 = x1;
			x1 = t; /* Swap X c
			 oordinates           */
			t = y2;
			y2 = y1;
			y1 = t; /* Swap Y c
			 oordinates           */
		}
		if (y2 > y1)
			yincr = 1;
		else
			yincr = -1;
		d = 2 * dy - dx; /* Initial deci
		 sion variable value  */
		Eincr = 2 * dy; /* Increment to
		 move to E pixel     */
		NEincr = 2 * (dy - dx); /* Increment to
		 move to NE pixel    */
		vg_set_pixel(x1, y1, color); /* Draw the fir
		 st point at (x1,y1)  */

		/* Incrementally determine the positions of
		 the remaining pixels */
		for (x1++; x1 <= x2; x1++) {
			if (d < 0)
				d += Eincr; /* Choose the E
				 astern Pixel         */
			else {
				d += NEincr; /* Choose the N
				 orth Eastern Pixel   */
				y1 += yincr; /* (or SE pixel
				 for dx/dy < 0!)     */
			}
			vg_set_pixel(x1, y1, color); /* Draw the poi
			 nt                   */
		}
	} else {
		/* We have a line with a slope between -1 a
		 nd 1 (ie: includes
		 * vertical lines). We must swap our x and
		 y coordinates for this.
		 *
		 * Ensure that we are always scan convertin
		 g the line from left to
		 * right to ensure that we produce the same
		 line from P1 to P0 as the
		 * line from P0 to P1.
		 */
		if (y2 < y1) {
			t = x2;
			x2 = x1;
			x1 = t; /* Swap X c
			 oordinates           */
			t = y2;
			y2 = y1;
			y1 = t; /* Swap Y c
			 oordinates           */
		}
		if (x2 > x1)
			yincr = 1;
		else
			yincr = -1;
		d = 2 * dx - dy; /* Initial deci
		 sion variable value  */
		Eincr = 2 * dx; /* Increment to
		 move to E pixel     */
		NEincr = 2 * (dx - dy); /* Increment to
		 move to NE pixel    */
		vg_set_pixel(x1, y1, color); /* Draw the fir
		 st point at (x1,y1)  */
		/* Incrementally determine the positions of
		 the remaining pixels */
		for (y1++; y1 <= y2; y1++) {
			if (d < 0)
				d += Eincr; /* Choose the E
				 astern Pixel         */
			else {
				d += NEincr; /* Choose the N
				 orth Eastern Pixel   */
				x1 += yincr; /* (or SE pixel
				 for dx/dy < 0!)     */
			}
			vg_set_pixel(x1, y1, color); /* Draw the poi
			 nt                   */
		}
	}
}

int vg_draw_pixmap(int xi, int yi, int h_size, int v_size, char * pixmap){
	unsigned int i;
	//draws the pixmap
	for(i=0;i<h_size;i++){
		unsigned int j;//counter
		for(j=0;j<v_size;j++){
			if(vg_set_pixel(xi+i,yi+j, *(pixmap+i+j*h_size))!=0){
				return 1;
			}
		}
	}
	//sets all the other pixels of the screen to black
	for(i=0;i<h_res;i++){
		unsigned int j;//counter
		for(j=0;j<v_res;j++){
			if(i<xi || j< yi || i>=xi+h_size || j>=yi+v_size){
				if(vg_set_pixel(i,j,0)!=0){
					return 1;
				}
			}
		}
	}
	return 0;
}

int vg_set_screen(unsigned long color){
	unsigned int i;//counter
	for(i=0;i<h_res;i++){
		unsigned int j;//counter
		for(j=0;j<v_res;j++){
			if(vg_set_pixel(i,j,color)!=0){
				return 1;
			}
		}
	}
	return 0;
}
