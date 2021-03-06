#include "kbc.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "utilities.h"
#include "i8042.h"
#include "i8254.h"

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

int kbc_c_to_ass(unsigned long * scancode){
	return kbc_cmd_read_ass(scancode);
}
