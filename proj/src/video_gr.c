#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include "video_gr.h"
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
static char * double_buffer;/*Double buffer*/
static char * mouse_buffer;//mouse buffer

static unsigned h_res; /* Horizontal screen resolution in pixels */
static unsigned v_res; /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

unsigned getMouse_buffer(){
	return (unsigned) mouse_buffer;
}

unsigned getDouble_buffer(){
	return (unsigned) double_buffer;
}

unsigned getVideo_mem(){
	return (unsigned) video_mem;
}

unsigned getH_res() {
	return h_res;
}

unsigned getV_res() {
	return v_res;
}

int vg_exit() {
	free(double_buffer);
	free(mouse_buffer);
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
			double_buffer=malloc(sizeof(char)*h_res*v_res*bits_per_pixel/8);
			mouse_buffer=malloc(sizeof(char)*h_res*v_res*bits_per_pixel/8);
			return video_mem;
		}
	}

}

int vg_set_pixel(int x, int y, unsigned int color) {
	if (x >= h_res || y >= v_res || x<0 || y<0) {//checks that the coordinates of the point are valid
		return 1;
	}
	else {
		int i;//counter
		for(i=0;i<(bits_per_pixel/8);i++){
			*(double_buffer + (x + (y * h_res)) * (bits_per_pixel / 8)+i) = (color & (0x0FF<<(i*8)))>>(i*8);
			*(mouse_buffer+(x + (y * h_res)) * (bits_per_pixel / 8)+i)=(color & (0x0FF<<(i*8)))>>(i*8);
		}
		return 0;
	}
}

int vg_set_mouse_pixel(int x, int y, unsigned int color){
	if (x >= h_res || y >= v_res || x<0 || y<0) {//checks that the coordinates of the point are valid
		return 1;
	}
	else {
		int i;//counter
		for(i=0;i<bits_per_pixel/8;i++){
			*(mouse_buffer+(x + (y * h_res)) * (bits_per_pixel / 8)+i)=(color & (0x0FF<<(i*8)))>>(i*8);
		}
		return 0;
	}
}


int vg_draw_square(int xi, int yi, int size,int color){
	int i;
	for(i=0;i<size;i++){
		int j;
		for(j=0;j<size;j++){
			if(vg_set_pixel(xi+i,yi+j,color)==1){
				return 1;
			}
		}
	}
	return 0;
}

int vg_draw_circle(int xi, int yi,int radius, int color){
	int i;
	for(i=xi-radius;i<xi+radius;i++){
		int j;
		for(j=yi-radius;j<yi+radius;j++){
			if((i-xi)*(i-xi)+(j-yi)*(j-yi)<=radius*radius){
				if(vg_set_pixel(i,j,color)!=0){
					return 1;
				}
			}
		}
	}

	 return 0;
}






int swap_double_video(){
	if(memcpy(video_mem,double_buffer,v_res*h_res*bits_per_pixel/8)!=NULL){
		return 0;
	}
	else{
		return 1;
	}
	return 0;
}

int swap_mouse_video(){
	if(memcpy(video_mem,mouse_buffer,v_res*h_res*bits_per_pixel/8)!=NULL){
		return 0;
	}
	else{
		return 1;
	}
	return 0;
}

int swap_mouse_double(){
	if(memcpy(double_buffer,mouse_buffer,v_res*h_res*bits_per_pixel/8)!=NULL){
		return 0;
	}
	else{
		return 1;
	}
	return 0;
}

int swap_double_mouse(){
	if(memcpy(mouse_buffer,double_buffer,v_res*h_res*bits_per_pixel/8)!=NULL){
		return 0;
	}
	else{
		return 1;
	}
	return 0;
}

int mouse_to_double(int x, int y){
	if(x>=h_res || y>=v_res || x<0 || y<0){
		return 1;
	}
	else{
		int i;//counter
		for(i=0;i<bits_per_pixel/8;i++){
			*(mouse_buffer+(x + (y * h_res)) * (bits_per_pixel / 8)+i)=*(double_buffer+(x + (y * h_res)) * (bits_per_pixel / 8)+i);
		}
	}
	return 0;
}







