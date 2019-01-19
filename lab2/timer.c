#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"

unsigned long timer0_counter=0;//global variable the number of messages in timer_test_int
int hook_id=0;//hook for handling interruptions
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

int timer_subscribe_int(void ) {
	int old_hook=hook_id;//new variable that preserves the value of the original hook
	if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id)==OK){
		if(sys_irqenable(&hook_id)==OK){
			return BIT(old_hook);
		}
	}
	return -1;
}

int timer_unsubscribe_int() {
	if(sys_irqdisable(&hook_id)==OK){
		if(sys_irqrmpolicy(&hook_id)==OK){
			return 0;
		}
	}
	return -1;
}

void timer_int_handler() {
	timer0_counter++;
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

int timer_display_conf(unsigned char conf) {
	//displays output
	if((conf & BIT(7))==0){
		printf("Output bit: 0\n");
	}
	else{
		printf("Output bit: 1\n");
	}
	//displays Null count
	if((conf & BIT(6))==0){
		printf("Null Count bit: 0\n");
	}
	else{
			printf("Null Count bit: 1\n");
		}
	//displays type of access
	if((conf & (BIT(5) | BIT(4)))>>4 ==0){
		printf("Type of access: Counter Latch Command\n");
	}
	else if((conf & (BIT(5) | BIT(4)))>>4 ==1){
			printf("Type of access: LSB\n");
		}
	else if((conf & (BIT(5) | BIT(4)))>>4 ==2){
				printf("Type of access: MSB\n");
			}
	else if((conf & (BIT(5) | BIT(4)))>>4 ==3){
				printf("Type of access: LSB followed by MSB\n");
			}
	//displays mode
	if(  (conf & (BIT(3)|BIT(2)|BIT(1)))>>1==0){
		printf("Mode: 0\n");
	}
	else if(  (conf & (BIT(3)|BIT(2)|BIT(1)))>>1==1){
			printf("Mode: 1\n");
		}
	else if(  (conf & (BIT(2)|BIT(1)))>>1==2){
				printf("Mode: 2\n");
			}
	else if(  (conf & (BIT(2)|BIT(1)))>>1==3){
				printf("Mode: 3\n");
			}
	else if(  (conf & (BIT(3)|BIT(2)|BIT(1)))>>1==4){
				printf("Mode: 4\n");
			}
	else if(  (conf & (BIT(3)|BIT(2)|BIT(1)))>>1==5){
				printf("Mode: 5\n");
			}
	//displays counting mode
	if((conf & BIT(0))==0){
		printf("Counting Mode: Binary\n");
	}
	else if ((conf & BIT(0))==1){
			printf("Counting Mode: BCD\n");
		}

	return 0;
}

int timer_test_square(unsigned long freq) {
	if(timer_set_square(0, freq)!=0){
		return 1;
	}
	return 0;
}

int timer_test_int(unsigned long time) {
	int ipc_status;//ipc_status
	message msg;//message
	int r;//variable that stores the error
	char irq_set=BIT(hook_id);
	if(timer_subscribe_int()==-1){
		printf("Error: It wasn't possible to subscribe the interruption\n");
		return -1;
	}
	int cicle_counter=0;//counts cicles between each message
	while( timer0_counter < time ) {
		/* You may want to use a different condition */
		 /* Get a request message. */
		 if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			 printf("driver_receive failed with: %d", r);
			 continue;
			 }
		 if (is_ipc_notify(ipc_status)) { /* received notification */
			 switch (_ENDPOINT_P(msg.m_source)) {
			 case HARDWARE: /* hardware interrupt notification */
				 if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					 cicle_counter++;
					 if((cicle_counter%60)==0){
						 printf("Message number %d\n",timer0_counter+1);
						 timer_int_handler();
					 }
					 }
			 break;
			 default:
				 break; /* no other notifications expected: do nothing */
			 }
			 } else { /* received a standard message, not a notification */
			 /* no standard messages expected: do nothing */
				 }
		 }
	timer_unsubscribe_int();
	return 0;
	
}

int timer_test_config(unsigned long timer) {
	if(timer<0 || timer >2){
		printf("Error: Timer Invalid\n");
			return 1;
		}

	unsigned char  conf;//configuration of the timer

	if (timer_get_conf(timer, & conf)!=0){
		printf("Error: It was not possible to obtain the timer configuration\n");
		return 1;}
	printf("Timer: %d\n", timer);//displays timer
	if (timer_display_conf( conf)!=0){
		return 1;}
	return 0;
}

