#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8042.h"
#include "i8254.h"
#include"kbd_functions.h"


extern unsigned long scancode;//global variable that is going to store the scancodes received
int kbd_test_scan_c(int * two_bytes){
	if(kbc_cmd_read(& scancode)==0){
		if(scancode==0xE0){//if scancode has two bytes
		 *two_bytes=1;
								 	 }
									else{
									 if(*two_bytes==1){
										 *two_bytes=2;
									 }
									 else{
										 *two_bytes=0;
									 }
								 	 }
								 if(*two_bytes==0){
									 if(IS_BREAKCODE(scancode)){
										 printf("Breakcode: 0x%x\n",scancode);
									 	 }
									 else{
										 printf("Makecode: 0x%x\n",scancode);
									 	 }
								 	 }
								 else if(*two_bytes==2){
									 if(IS_BREAKCODE(scancode)){
									 	printf("Breakcode: 0xE0%x\n",scancode);
									 	}
									 else{
								        printf("Makecode: 0XE0%x\n",scancode);
									 	 }
									 }
								 return 0;
								 }
	return 1;
}

int kbd_test_scan(unsigned short ass) {
	int ipc_status;//ipc_status
		message msg;//message
		int r;//variable that stores the error
		char irq_set=BIT(kbd_hook_id);
		if(kbd_subscribe_int()==1){
			printf("Error: It wasn't possible to subscribe the interruption\n");
			return -1;
		}



		scancode=0;//sets the initial value of scancode to 0
		int two_bytes=0;//this variable gives information about the position of the current bit in the scancode
		while(scancode !=0x81) {
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
						 if(ass==0){
							 if(kbd_test_scan_c(& two_bytes)!=0){
								 printf("Error: It wasn't possible to obtain the scancodes\n");
								 kbd_unsubscribe_int();
								 return 1;
							 }
						 }
						 else{
							 if(kbd_test_scan_ass(& two_bytes)!=0){
								printf("Error: It wasn't possible to obtain the scancodes\n");
								 kbd_unsubscribe_int();
								return 1;
							 }
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
		kbd_unsubscribe_int();
		return 0;
}
int kbd_test_leds(unsigned short n, unsigned short *leds) {
	unsigned int i;//counter
	for(i=0;i<n;i++){
		if((leds[i]<0) || (leds[i]>2)){
			printf("Error:Invalid arguments.\n All elements of the sequence must be between 0 and 2\n");
			return 1;
		}
	}

    int ipc_status;//ipc_status

    message msg;//message

    int r;//variable that stores the error

    char irq_set=BIT(timer_hook_id);

    if(timer_subscribe_int()==-1){

            printf("Error: It wasn't possible to subscribe the interruption\n");

            return -1;

    }
    unsigned int * led_state;//creates array that has the state of the leds. Initially assumes that are all turned off
    led_state=malloc(3*sizeof(unsigned int));
    led_state[0]=0;
    led_state[1]=0;
    led_state[2]=0;

    int counter=0;////counts the number of ticks

    while( counter < 60*n ) {

             /* Get a request message. */

             if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {

                     printf("driver_receive failed with: %d", r);

                     continue;

                     }

             if (is_ipc_notify(ipc_status)) { /* received notification */

                     switch (_ENDPOINT_P(msg.m_source)) {

                     case HARDWARE: /* hardware interrupt notification */

                             if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

                                     if((counter%60)==0){
                                    	 unsigned long * cmds;
                                    	 cmds=malloc(2*sizeof(unsigned long));//variable that is going to store the commands that are going to be written to the kbd
                                    	 cmds[0]=KBD_LEDS;//The first command is 0xED
                                    	 if(leds[counter/60]==0){
                                    		 led_state[0]=(led_state[0]+1)%2;
                                    		 cmds[1]=KBD_LED(2)*led_state[2]+KBD_LED(1)*led_state[1]+KBD_LED(0)*led_state[0];//if state is 0 cmd is 0
                                    	 }
                                    	 else if(leds[counter/60]==1){
                                    		 led_state[1]=(led_state[1]+1)%2;
                                    		 cmds[1]=KBD_LED(2)*led_state[2]+KBD_LED(1)*led_state[1]+KBD_LED(0)*led_state[0];;//if state is 0 cmd is 0
                                    		 //printf("1:0x%x\n",cmds[1]);
                                    	      }
                                    	 else {
                                    		 led_state[2]=(led_state[2]+1)%2;
                                    		 cmds[1]=KBD_LED(2)*led_state[2]+KBD_LED(1)*led_state[1]+KBD_LED(0)*led_state[0];;//if state is 0 cmd is 0
                                    		 //printf("2:0x%x\n",cmds[1]);
                                    	 }
                                    	 if(kbd_cmd_write(2, cmds)!=0){
                                    		 free(cmds);
                                    		 free(led_state);
                                    		 timer_unsubscribe_int();
                                    		 printf("Error: It was not possible to switch the KBD LEDS\n");
                                    		 return 1;
                                    	 }
                                    	 free(cmds);
                                     }

                                 counter=counter+1;

                             }

                     break;

                     default:

                             break; /* no other notifications expected: do nothing */

                     }

                     } else { /* received a standard message, not a notification */

                     /* no standard messages expected: do nothing */

                             }

             }
    free(led_state);
    timer_unsubscribe_int();

    return 0;

}


int kbd_test_timed_scan(unsigned short n) {
	int ipc_status;//ipc_status
			message msg;//message
			int r;//variable that stores the error
			char irq_set_timer=BIT(timer_hook_id);
			char irq_set_kbd=BIT(kbd_hook_id);
			if(timer_subscribe_int()==-1){
				printf("Error: It wasn't possible to subscribe the timer interruption\n");
				return 1;
			}
			if(kbd_subscribe_int()==-1){
				printf("Error: It wasn't possible to subscribe the keyboard interruption\n");
				return 1;
			}


			scancode=0;//sets the initial value of scancode to 0
			int two_bytes=0;//this variable gives information about the position of the current bit in the scancode

			unsigned int counter=0;//counts the number of ticks
			while((scancode!=0x81) && (counter<=60*n)) {
				/* You may want to use a different condition */
				 /* Get a request message. */
				 if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
					 printf("driver_receive failed with: %d", r);
					 continue;
					 }
				 if (is_ipc_notify(ipc_status)) { /* received notification */
					 switch (_ENDPOINT_P(msg.m_source)) {
					 case HARDWARE: /* hardware interrupt notification */
						 if(msg.NOTIFY_ARG & irq_set_timer){/*subscribed timer interrupt*/
							 	 counter=counter+1;
						 }
						 if (msg.NOTIFY_ARG & irq_set_kbd) { /* subscribed kbd interrupt */
							 if(kbd_test_scan_c(& two_bytes)!=0){
								 printf("Error: It wasn't possible to obtain the scancodes\n");
								 kbd_unsubscribe_int();
								 timer_unsubscribe_int();
								 return 1;
							 }
							 counter=0;
						 }

					 break;
					 default:
						 break; /* no other notifications expected: do nothing */
					 }
					 } else { /* received a standard message, not a notification */
					 /* no standard messages expected: do nothing */
						 }
				 }
			if(counter>60*n){//if more than n seconds have passed
				printf("Exit Timeout\n");
			}
			kbd_unsubscribe_int();
			timer_unsubscribe_int();
			return 0;
}
