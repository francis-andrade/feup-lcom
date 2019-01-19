#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include <stdlib.h>
#include <stdint.h>
#include "i8042.h"
#include "i8254.h"
#include "peripherals.h"
#include "vbe.h"
#include "video_gr.h"
#include "read_xpm.h"
#include "Sprite.h"

void *test_init(unsigned short mode, unsigned short delay) {

	if (vg_init(mode) == NULL) {
		printf("Error: It wasn't possible to set the desired mode\n");
		return NULL;
	}
	vbe_mode_info_t vbe_config;
	if (vbe_get_mode_info(mode, &vbe_config)!=0) {
		printf("Error: It wasn't possible to obtain the VRAM physical address\n");
		return NULL;
	}

	int ipc_status;
	message msg;
	int r; //variable that stores the error
	char irq_set = BIT(timer_hook_id);

	if (timer_subscribe_int() == -1) {
		vg_exit();
		printf("Error: It wasn't possible to subscribe the timer\n");
		return NULL;
	}

	int counter = 0;
	while (counter < delay * 60) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) {
					counter++;
					printf("%d ", counter);
				}
				break;
			default:
				break;
			}
		}
	}

	if (timer_unsubscribe_int() != 0) {
		vg_exit();
		printf("Error: It wasn't possible to unsubscribe the timer\n");
		return NULL;
	}

	if (vg_exit() != 0) {
		printf(	"Error: It wasn't possible to return to the Default Minix 3 text mode\n");
		return NULL;
	}
	printf("\nVRAM Physical Address: 0x%X\n", vbe_config.PhysBasePtr);
	return vbe_config.PhysBasePtr;

}

int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color) {
	if (vg_exit() != 0) {
		printf("Error: It wasn't possible to return to the Default Minix 3 text mode\n");
		return 1;
	}
	if (vg_init(VBE_GRAPHIC_MODE) == NULL) {
		vg_exit();
		printf("Error: Failed to initialize the graphic mode\n");
		return 1;
	}

	if ((x < 0) || (y < 0) || (size < 0) || (x + size >= getH_res() || (y + size >= getV_res()))) {
		vg_exit();
		printf("Error: The user typed a wrong input\n");
		return 1;
	}


	unsigned int i; //counter
	for (i = x; i < x + size; i++) {
		unsigned int j; //counter
		for (j = y; j < y + size; j++) {
			if (vg_set_pixel(i, j, color) != 0) {
				vg_exit();
				printf("Error: It wasn't possible to set a pixel\n");
				return 1;
			}
		}
	}

	int ipc_status; //ipc_status
	message msg; //message
	int r; //variable that stores the error
	int kbd_hook_id = KBC_IRQ; //kbd_hook
	long irq_set = BIT(kbd_hook_id);
	if (kbd_subscribe_int(&kbd_hook_id) == -1) {
		printf("Error: It wasn't possible to subscribe the keyboard\n");
		return 1;
	}

	long scancode = 0; //scancode of the key that the user is going to press
	while (scancode != 0x81) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) {
					kbc_cmd_read(&scancode);
				}
				break;
			default:
				break;
			}
		}
	}
	if (kbd_unsubscribe_int(&kbd_hook_id)) {
		vg_exit();
		printf("Error: It wasn't possible to unsubscribe the kbd\n");
		return 1;
	}
	if (vg_exit() != 0) {
		printf("Error: It wasn't possible to return to the Default Minix 3 text mode\n");
		return 1;
	}
	return 0;

}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {

	if (vg_exit() != 0) {
		printf("Error: It wasn't possible to return to the Default Minix 3 text mode\n");
		return 1;
	}
	if (vg_init(VBE_GRAPHIC_MODE) == NULL) {
		vg_exit();
		printf("Error: Failed to initialize the graphic mode\n");
		return 1;
	}
	if ((xi < 0) || (yi < 0) || (xf < 0) || (yf < 0) || (xi >= getH_res())
			|| (xf >= getH_res()) || (yi >= getV_res()) || (yf >= getV_res())) {
		vg_exit();
		printf("Error: The user typed a wrong input\n");
		return 1;
	}
	vg_draw_line(xi, yi, xf, yf, color);

	int ipc_status; //ipc_status
	message msg; //message
	int r; //variable that stores the error
	int kbd_hook_id = KBC_IRQ; //kbd_hook
	long irq_set = BIT(kbd_hook_id);
	if (kbd_subscribe_int(&kbd_hook_id) == -1) {
		printf("Error: It wasn't possible to subscribe the keyboard\n");
		return 1;
	}

	long scancode = 0; //scancode of the key that the user is going to press
	while (scancode != 0x81) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) {
					kbc_cmd_read(&scancode);
				}
				break;
			default:
				break;
			}
		}
	}
	if (kbd_unsubscribe_int(&kbd_hook_id)) {
		vg_exit();
		printf("Error: It wasn't possible to unsubscribe the kbd\n");
		return 1;
	}
	if (vg_exit() != 0) {
		printf(	"Error: It wasn't possible to return to the Default Minix 3 text mode\n");
		return 1;
	}

	return 0;

}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {

	if (vg_exit() != 0) {
		printf("Error: It wasn't possible to return to the Default Minix 3 text mode\n");
		return 1;
	}
	if (vg_init(VBE_GRAPHIC_MODE) == NULL) {
		vg_exit();
		printf("Error: Failed to initialize the graphic mode\n");
		return 1;
	}
	if(xi>=getH_res() || yi>=getV_res()){
		vg_exit();
		printf("Error: The user typed a wrong input\n");
		return 1;
	}
	int width, height;//width and height pixmap
	char * map=read_xpm(xpm, & width, & height);
	if(map==NULL){
		vg_exit();
		printf("Error: It wasn't possible to obtain the pixmap\n");
		return 1;
	}
	if(vg_draw_pixmap(xi, yi, width, height, map)!=0){
		vg_exit();
		printf("Error: It wasn't possible to draw the pixmap\n");
	}
	int ipc_status; //ipc_status
	message msg; //message
	int r; //variable that stores the error
	int kbd_hook_id = KBC_IRQ; //kbd_hook
	long irq_set = BIT(kbd_hook_id);
	if (kbd_subscribe_int(&kbd_hook_id) == -1) {
		printf("Error: It wasn't possible to subscribe the keyboard\n");
		return 1;
	}

	long scancode = 0; //scancode of the key that the user is going to press
	while (scancode != 0x81) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) {
					kbc_cmd_read(&scancode);
				}
				break;
			default:
				break;
			}
		}
	}


	if (kbd_unsubscribe_int(&kbd_hook_id)) {
		vg_exit();
		printf("Error: It wasn't possible to unsubscribe the kbd\n");
		return 1;
	}
	if (vg_exit() != 0) {
		printf("Error: It wasn't possible to return to the Default Minix 3 text mode\n");
		return 1;
	}

	return 0;

}



int test_move(unsigned short xi, unsigned short yi, char *xpm[], unsigned short hor, short delta, unsigned short time) {

	if (vg_exit() != 0) {
		printf("Error: It wasn't possible to return to the Default Minix 3 text mode\n");
		return 1;
	}
	if (vg_init(VBE_GRAPHIC_MODE) == NULL) {
		vg_exit();
		printf("Error: Failed to initialize the graphic mode\n");
		return 1;
	}


	int ipc_status; //ipc_status
	message msg; //message
	int r; //variable that stores the error
	int kbd_hook_id = KBC_IRQ; //kbd_hook
	long irq_set = BIT(kbd_hook_id);
	if (kbd_subscribe_int(&kbd_hook_id) == -1) {
		printf("Error: It wasn't possible to subscribe the keyboard\n");
		return 1;
	}

	char irq_set_timer = BIT(timer_hook_id);
	if (timer_subscribe_int() == -1) {
		vg_exit();
		printf("Error: It wasn't possible to subscribe the timer\n");
		return NULL;
	}

	Sprite * sp;//sprite that is going to represent the xpm
	if(hor==0){
		sp=create_sprite(xpm,xi, yi, 0, delta/time);
	}
	else{
		sp=create_sprite(xpm,xi, yi, delta/time, 0);
	}
	long scancode = 0; //scancode of the key that the user is going to press
	long counter=0;//counts the number of timer interruptions(ticks)
	while (scancode != 0x81 && counter<time*60) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set_timer) {
					if(animate_sprite(sp)!=0){
						break;
					}
					counter++;
				}
				if (msg.NOTIFY_ARG & irq_set) {
					kbc_cmd_read(&scancode);
				}
				break;
			default:
				break;
			}
		}
	}

	destroy_sprite(sp);
	sp=NULL;
	if (timer_unsubscribe_int() != 0) {
		vg_exit();
		printf("Error: It wasn't possible to unsubscribe the timer\n");
		return NULL;
	}

	if (kbd_unsubscribe_int(&kbd_hook_id)) {
		vg_exit();
		printf("Error: It wasn't possible to unsubscribe the kbd\n");
		return 1;
	}

	if (vg_exit() != 0) {
		printf("Error: It wasn't possible to return to the Default Minix 3 text mode\n");
		return 1;
	}
	return 0;


}

int test_controller() {

	uint8_t * init=lm_init();//variable that is going to initialize the low memory area
	if(init==NULL){
		printf("Error: It wasn't possible to allocate memory in the lower area");
		return 1;
	}
	vbe_info_block_t vbe_config;//configuration of the controller information
	if (vbe_get_info_block( &vbe_config)!=0) {
		printf("Error: It wasn't possible to obtain the VRAM physical address\n");
		return NULL;
	}
	printf("\nController Capabilities:\n");
	if(vbe_config.Capabilities[0] & BIT(0) == 0){
		printf("-DAC is fixed width, with 6 bits per primary color\n");
	}
	else{
		printf("-DAC width is switchable to 8 bits per primary color\n");
	}
	if(vbe_config.Capabilities[0] & BIT(1) == 0){
		printf("-Controller is VGA compatible\n");
	}
	else{
		printf("-Controller is not VGA compatible\n");
	}
	if(vbe_config.Capabilities[0] & BIT(2) == 0){
		printf("-Normal RAMDAC operation\n");
	}
	else{
		printf("-When programming large blocks of information to the RAMDAC,\n use the blank bit in Function 09h.\n");
	}
	printf("\nMode numbers:\n");
	int convert=PB2OFF(vbe_config.VideoModePtr)+((vbe_config.VideoModePtr>>12) & 0xFFFF0000);//converts from physical address to linear address
	init=init+convert;
	uint16_t * video_mode=init;//variable that points to the list of mode numbers
	unsigned short counter=0;//counts the number of modes
	while(*video_mode!=0x0FFFF){
		counter++;
		printf("%d:0x%X ",counter, *video_mode);
		video_mode++;
	}
	if(counter==0){
		printf("There are no video modes. This application is a stub\n");
	}
	printf("\n\nSize of VRAM memory(in KB): %d\n",vbe_config.TotalMemory*64);


}

