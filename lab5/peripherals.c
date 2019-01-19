#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "peripherals.h"
#include "utilities.h"
#include "i8042.h"
#include "i8254.h"


int timer_hook_id=0;


//generic functions
void ignore_interrupts(unsigned char hook_bit) {
	int ipc_status;
	message msg;
	int r; //variable that stores the error
	int interrupt_found=0;//variable that tells if an interrupt has already been found
	unsigned int i = 0;
	while ((i < 10) && (interrupt_found==0)) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source) )
			{
			case HARDWARE:
					if(msg.NOTIFY_ARG & BIT(hook_bit)){
						interrupt_found=1;
					}
				break;
			default:
				break;
			}
		}
		i++;
	}
}

//timer functions
int timer_subscribe_int(void) {
	int old_hook = timer_hook_id; //new variable that preserves the value of the original hook
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id) == OK) {
		if (sys_irqenable(&timer_hook_id) == OK) {
			return BIT(old_hook);
		}
	}
	return -1;
}

int timer_unsubscribe_int() {
	if (sys_irqdisable(&timer_hook_id) == OK) {
		if (sys_irqrmpolicy(&timer_hook_id) == OK) {
			return 0;
		}
	}
	return -1;
}
//keyboard functions

int kbd_subscribe_int(unsigned int * kbd_hook_id) {

        int old_hook = *kbd_hook_id; //new variable that preserves the value of the original hook

        if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, kbd_hook_id) == OK) {

                if (sys_irqenable(kbd_hook_id) == OK) {


                        return BIT(old_hook);

                }

        }

        return -1;

}

int kbd_unsubscribe_int(unsigned int * kbd_hook_id) {

        if (sys_irqdisable(kbd_hook_id) == OK) {

                if (sys_irqrmpolicy(kbd_hook_id) == OK) {

                           return 0;

                }

        }

        return -1;

}


int kbc_cmd_read(unsigned long *data){

        unsigned long stat;//variable that will store the status of the register

        unsigned long data_temporary;//variable that will store the temporary kbd output

        unsigned int i=0;

        while( i<10 ) {

                if(sys_inb(KBC_STAT_REG, &stat)!=OK){

                        return 1;

                }

                if( stat & KBC_OBF ) {

                        if(sys_inb(KBC_OUT_BUF, &data_temporary)!=OK){

                                return 1;

                        }

                        if ( (stat &(KBC_PAR_ERROR | KBC_TO_ERROR )) == 0 ){

                                *data=data_temporary;

                                return 0;

                        }

                        else{

                                return 1;

                        }

                }

        tickdelay(micros_to_ticks(DELAY_US));
        i++;
        }
        return 1;

}

int kbc_cmd_issue(unsigned long cmd){//0x64

        unsigned long stat;//variable that will store the status of the register
        unsigned int i=0;
        while(i<10){

                if(sys_inb(KBC_STAT_REG, &stat)!=OK){

                        return 1;

                }

                if((stat & KBC_IBF)==0){

                        if(sys_outb(KBC_CMD_REG, cmd)==OK){

                                return 0;

                        }

                        else{

                                return 1;

                        }

                }
        i++;
        tickdelay(micros_to_ticks(20000));

        }
        return 1;

}

int kbd_cmd_issue(unsigned long cmd){//0x60

        unsigned long stat;//variable that will store the status of the register
        unsigned int i=0;
        while(i<10){

                if(sys_inb(KBC_STAT_REG, &stat)!=OK){

                        return 1;

                }

                if((stat & KBC_IBF)==0){

                        if(sys_outb(KBC_IN_BUF, cmd)==OK){

                                return 0;

                        }

                        else{

                                return 1;

                        }

                }
        i++;
        tickdelay(micros_to_ticks(DELAY_US));

        }
        return 1;

}



int kbd_cmd_write(unsigned int n, unsigned long * cmds){
	unsigned int i=0;//counter
	unsigned long data=0;//variable that is going to store the input from kbd
	while(i<n){
		if(kbd_cmd_issue(cmds[i])!=0){
			return 1;
		}
		unsigned long stat;
		tickdelay(micros_to_ticks(DELAY_US));
		if(sys_inb(KBC_OUT_BUF, & data)!=OK){
			return 1;
		}
		tickdelay(micros_to_ticks(DELAY_US));
		if(data == CKBD_ERROR){
			i=0;//restart loop
		}
		else if(data!=CKBD_RESEND){
			i=i+1;//eveything ok continue loop
		}

	}
	return 0;
}


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

