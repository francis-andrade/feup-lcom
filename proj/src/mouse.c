#include "mouse.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "video_gr.h"
#include "utilities.h"
#include "i8042.h"
#include "i8254.h"

//mouse functions

int mouse_subscribe_int(unsigned int * mouse_hook_id){
	int old_hook = *mouse_hook_id; //new variable that preserves the value of the original hook
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, mouse_hook_id) == OK) {
		if (sys_irqenable(mouse_hook_id) == OK) {
			return BIT(old_hook);
		}
	}
	return -1;
}

int mouse_unsubscribe_int(unsigned int * mouse_hook_id){
	if (sys_irqdisable(mouse_hook_id) == OK) {
			if (sys_irqrmpolicy(mouse_hook_id) == OK) {
				return 0;
			}
		}

	return -1;
}

int mouse_cmd_write(unsigned long cmd){
	unsigned int i;
	for(i=0;i<10;i++){

        if(kbc_cmd_issue(KBC_WRITE_TO_MOUSE)==1){//0x64

                return 1;

        }

        if(kbd_cmd_issue(cmd)==1){//0x60

                return 1;

        }

        unsigned long stat;
        if(kbc_cmd_read(&stat)==1){
        	return 1;
        }
        if(stat==CKBD_ACK){
        	return 0;
        }
	}
	return 1;
}

int mouse_cmd_read(unsigned long * data){
	unsigned int i=0;
	while(i<10){
		unsigned long stat;
		if(sys_inb(KBC_STAT_REG, &stat)!=OK){
			return 1;
		}
		tickdelay(micros_to_ticks(DELAY_US));
		if(kbc_cmd_read(data)==0){
				 if((stat & BIT(5))==BIT(5)){
					 return 0;
				 }
		}

		 i++;
    }
	return 1;
}

int mouse_get_packet(unsigned long *next_byte, unsigned char * packet) {
	unsigned long data;
	if (kbc_cmd_read(&data) != 0) {
		return 1;
	}

	if ((*next_byte) == 0) {
		if ((data & BIT(3)) == BIT(3)) {
			packet[0] = data;

			(*next_byte) = (*next_byte + 1) % 3;
		} else {

		}
	} else if ((*next_byte) == 1) {
		packet[1] = data;
		(*next_byte) = (*next_byte + 1) % 3;

	} else {
		packet[2] = data;
		(*next_byte) = (*next_byte + 1) % 3;
	}
	return 0;
}

void mouse_convert_packet(unsigned char *array_packet, mouse_packet_t * packet){
	packet->lb=array_packet[0] & BIT(0);
	packet->mb=(array_packet[0] & BIT(2))>>2;
	packet->rb=(array_packet[0] & BIT(1))>>1;
	if((array_packet[0] & BIT(4))== BIT(4)){
		packet->x=-conv_to_decimal(two_complement_sym(array_packet[1]));
	}
	else{
		packet->x=(int) array_packet[1];
	}
	if((array_packet[0] & BIT(5))==BIT(5)){
		packet->y=-conv_to_decimal(two_complement_sym(array_packet[2]));
	}
	else{
		packet->y=(int) array_packet[2];
	}
}



