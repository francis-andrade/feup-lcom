#include "timer.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "utilities.h"
#include "i8042.h"
#include "i8254.h"

int timer_hook_id=0;


//generic functions

void ignore_interrupts(unsigned char hook) {
	message msg;
	int ipc_status;
	int r; //variable that stores the error
	int interrupt_found = 0; //variable that tells if an interrupt has already been found
	unsigned int i = 0;
	while ((i < 10) && (interrupt_found == 0)) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if  (   is_ipc_notify(ipc_status ) ) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & BIT(hook)) {
						 interrupt_found = 1;
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

/**
 * @brief sets the timer frequency
 * @param timer -> timer
 * @param freq -> frequence
 * @return 1 in case of success, o otherwise
 * */
int timer_set_square(unsigned long timer, unsigned long freq) {
        if(freq<1 || freq>TIMER_FREQ || TIMER_FREQ/freq > 0xFFFF){
                printf("Error: Invalid frequence\n");
                return 1;
        }
        unsigned char conf;//configuration of timer
        if(timer_get_conf(timer, & conf)!=0){
                return 1;
        }
        unsigned long cmd=(timer<<6)|TIMER_LSB_MSB | (0x0F & conf);
        if(sys_outb(TIMER_CTRL, cmd)!=OK){
                return 1;
        }
        if(sys_outb(TIMER_0+timer,(TIMER_FREQ/freq) )!=OK){
                return 1;
        }
        if(sys_outb(TIMER_0+timer,(TIMER_FREQ/freq)>>8)!=OK){
                        return 1;
        }
        return 0;

}

int timer_get_conf(unsigned long timer, unsigned char *st) {

        if(timer<0 || timer >2){

                        return 1;

                }

                unsigned long cmd;//control word

                unsigned long readword;//variable that is going to store the configuration of the timer

                cmd= 0 | TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

                if(sys_outb(TIMER_CTRL,cmd)!=OK){

                        return 1;

                }

                if(sys_inb(TIMER_0+timer,& readword)!=OK){

                        return 1;

                }

                *st=(unsigned char) readword;

                return 0;

}


