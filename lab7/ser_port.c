#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "ser_port.h"
#include "queue.h"

static queue_t * transmit_queue;
static queue_t * receive_queue;

ser_conf_t get_conf(unsigned short base_addr){
	ser_conf_t r;//configuration that is going to be returned
	unsigned long lcr;//variable that is going to store the lcr register
	if(sys_inb(base_addr+SER_LCR, & lcr)!=OK){
		return r;
	}
	r.bits_per_char=5+(lcr & (BIT(0) | BIT(1)));
	printf("lcr: %x\n",lcr);
	r.stop_bits=1+((lcr & BIT(2))>>2);
	if((lcr & BIT(3))==0){
		strcpy(r.parity,"None");
	}
	else if((lcr & BIT(5))==0){
		if((lcr & BIT(4))==0){
			strcpy(r.parity,"Odd");
		}
		else{
			strcpy(r.parity,"Even");
		}
	}
	else{
		if((lcr & BIT(4))==0){
			strcpy(r.parity,"1");
		}
		else{
			strcpy(r.parity,"0");
		}
	}
	if(sys_outb(base_addr+SER_LCR,(lcr | SER_DLAB))!=OK){
		ser_conf_t rr;
		return rr;
	}
	unsigned long dll, dlm;//LSB and MSB DL register
	if(sys_inb(base_addr+SER_DLL, & dll)!=OK){
		ser_conf_t rr;
		return rr;
	}
	if(sys_inb(base_addr+SER_DLM, &dlm)!=OK){
		ser_conf_t rr;
		return rr;
	}
	if(sys_outb(base_addr+SER_LCR,lcr)!=OK){
		ser_conf_t rr;
		return rr;
	}
	r.rate=(dlm<<8) | dll;
	r.rate=115200/r.rate;
	unsigned long ier;//variable that is going to store the ier register
	if(sys_inb(base_addr+SER_IER, & ier)!=OK){
		ser_conf_t rr;
		return rr;
	}
	if((ier & BIT(0))==BIT(0)){
		r.rec_data_intpt=1;
	}
	else{
		r.rec_data_intpt=0;
	}

	if((ier & BIT(1))==BIT(1)){
		r.trans_empty_intpt=1;
	}
	else{
		r.trans_empty_intpt=0;
	}

	if((ier & BIT(2))==BIT(2)){
		r.rec_line_stat_intpt=1;
	}
	else{
		r.rec_line_stat_intpt=0;
	}
	return r;
}

int ser_set_bits_per_char(unsigned short base_addr,int bits){
	unsigned long lcr;//variable that is going to store the lcr register
	if(sys_inb(base_addr+SER_LCR, & lcr)!=OK){
		return 1;
	}
	if(bits==5){
		lcr=((lcr>>2)<<2);
	}
	else if(bits==6){
		lcr=(((lcr>>2)<<2) | BIT(0));
	}
	else if(bits==7){
		lcr=((lcr>>2)<<2) |BIT(1);
	}
	else if(bits==8){
		lcr=((lcr>>2)<<2) | (BIT(1) | BIT(0));
	}
	else{
		return 1;
	}
	if(sys_outb(base_addr+SER_LCR, lcr)!=OK){
		return 1;
	}
	return 0;
}

int ser_set_stop_bits(unsigned short base_addr,int bits){
	unsigned long lcr;//variable that is going to store the lcr register
	if(sys_inb(base_addr+SER_LCR, & lcr)!=OK){
		return 1;
	}
	if(bits==1){
		lcr=lcr & (0x0FB);
	}
	else if(bits==2){
		lcr=lcr | BIT(2);
	}
	else{
		return 1;
	}
	if(sys_outb(base_addr+SER_LCR, lcr)!=OK){
		return 1;
	}
	return 0;
}

int ser_set_parity(unsigned short base_addr,int parity){
	unsigned long lcr;//variable that is going to store the lcr register
	if(sys_inb(base_addr+SER_LCR, & lcr)!=OK){
		return 1;
	}
	if(parity==-1){
		lcr=lcr & 0x0F7;
	}
	else if(parity==0){
		lcr=(lcr & 0x0DF) | 0x18;
	}
	else if(parity==1){
		lcr=(lcr & 0x0CF) | 0x08;
	}
	else{
		return 1;
	}
	if(sys_outb(base_addr+SER_LCR, lcr)!=OK){
		return 1;
	}
	return 0;
}

int ser_set_rate(unsigned short base_addr,int rate){
	unsigned long lcr;//variable that is going to store the lcr register
	if(sys_inb(base_addr+SER_LCR, & lcr)!=OK){
		return 1;
	}
	if(sys_outb(base_addr+SER_LCR,lcr | SER_DLAB)!=OK){
		return 1;
	}
	rate=115200/rate;
	if(sys_outb(base_addr+SER_DLL, (rate & 0x0FF))!=OK){
		return 1;
	}
	if(sys_outb(base_addr+SER_DLM, rate>>8)!=OK){
		return 1;
	}
	if(sys_outb(base_addr+SER_LCR,lcr)!=OK){
		return 1;
	}
	return 0;
}

int ser_set_conf(unsigned short base_addr, int bits, int stop, int parity, int rate){
	if(ser_set_bits_per_char(base_addr, bits)!=0){
			return 1;
	}

	if(ser_set_stop_bits(base_addr, stop)!=0){
		return 1;
	}

	if(ser_set_parity(base_addr, parity)!=0){
		return 1;
	}

	if(ser_set_rate(base_addr, rate)!=0){
		return 1;
	}
	return 0;
}

int ser_check_lsr_error(unsigned short base_addr){
	unsigned long lsr;//variable that is going to store the lsr register
	if(sys_inb(base_addr+SER_LSR, & lsr)!=OK){
		return -1;
	}
	if(lsr & (SER_OVERRUN_ERR | SER_PARITY_ERR | SER_FRAME_ERR)){
		return 1;
	}
	else{
		return 0;
	}
}

int ser_disable_int(unsigned short base_addr){
	unsigned long ier;
	if(sys_inb(SER_COM1_ADDR+SER_IER, & ier)!=OK){
		return 1;
	}
	ier = (ier & 0xF8);
	if(sys_outb(SER_COM1_ADDR+SER_IER, ier)!=OK){
		return 1;
	}
	return 0;
}

//POLLING

int ser_receive_char_poll(unsigned short base_addr, char * c, int num_cicles){
 if(ser_check_lsr_error(base_addr)!=0){
	 printf("LSR Error\n");
	 return 1;
}
 unsigned long lsr;//variable that is going to store the lsr register
 if(sys_inb(base_addr+SER_LSR, & lsr)!=OK){
	 return 1;
 }
 int i=0;//counter
 if(num_cicles==0){
	 i=-1;
 }
 while(!(lsr & SER_REC_RDY) && i<num_cicles){
	 if(sys_inb(base_addr+SER_LSR, & lsr)!=OK){
		 return 1;
	 }
	 tickdelay(micros_to_ticks(DELAY_US));
	 if(num_cicles!=0){
		 i++;
	 }
 }
 if(i==num_cicles){
	 return 1;
 }
 if(sys_inb(base_addr+SER_DATA,(unsigned long *) c)!=OK){
	 return 1;
 }

 return 0;
}


int ser_send_char_poll(unsigned short base_addr, char c, int num_cicles){
 unsigned long lsr;//variable that is going to store the lsr register
 if(sys_inb(base_addr+SER_LSR, & lsr)!=OK){
	 return 1;
 }
 int i=0;//counter
 if(num_cicles==0){
	 i=-1;
 }
 while(!(lsr & SER_TX_RDY) && i<num_cicles){
	 if(sys_inb(base_addr+SER_LSR, & lsr)!=OK){
		 return 1;
	 }
	 tickdelay(micros_to_ticks(DELAY_US));
	 if(num_cicles!=0){
		 i++;
	 }
 }
 if(i==num_cicles){
	 return 1;
 }
 if(sys_outb(base_addr+SER_DATA, c)!=OK){
	 return 1;
 }

 return 0;
}

int ser_send_string_poll(unsigned short base_addr,unsigned long bits, unsigned long stop, long parity, unsigned long rate, char string[]){
	if(ser_disable_int(base_addr)!=0){
		return 1;
	}
	if(ser_set_conf(base_addr, bits, stop, parity, rate)!=0){
		return 1;
	}

	int i=0;//counter
	do{
		if(ser_send_char_poll(base_addr,string[i], 0)!=0){
			return 1;
		}
		printf("%c",string[i-1]);
		i++;
	}while(string[i-1]!=0);
	return 0;
}



int ser_receive_string_poll(unsigned short base_addr,unsigned long bits, unsigned long stop, long parity, unsigned long rate){
	if(ser_disable_int(base_addr)!=0){
			return 1;
	}

	if(ser_set_conf(base_addr, bits, stop, parity, rate)!=0){
		return 1;
	}


	char c;//character that is going to be received
	printf("Star receiving\n");
	do{
		if(ser_receive_char_poll(base_addr,& c, 10000)!=0){
			printf("Error receiving char\n");
			return 1;
		}
		printf("char: %c %d ",c, c);
	}while(c!=0);
	return 0;
}











































int ser_send_strings_poll(unsigned short base_addr, unsigned long bits, unsigned long stop, long parity, unsigned long rate, int stringc, char *strings[]){
	if(ser_set_bits_per_char(base_addr, bits)!=0){
		return 1;
	}

	if(ser_set_stop_bits(base_addr, stop)!=0){
		return 1;
	}

	if(ser_set_parity(base_addr, parity)!=0){
		return 1;
	}

	if(ser_set_rate(base_addr, rate)!=0){
		return 1;
	}
	int i;//counter
	for(i=0;i<stringc;i++){
		if(ser_send_string_poll(base_addr,bits, stop, parity, rate, strings[i])!=0){
			return 1;
		}
		if(ser_send_char_poll(base_addr, ' ',0)!=0){
			return 1;
		}
	}
	return 0;

}

int ser_subscribe_int(unsigned int base, unsigned int * ser_hook_id) {

        int old_hook = *ser_hook_id; //new variable that preserves the value of the original hook

        if(base==1){
        	if (sys_irqsetpolicy(SER_COM1_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, ser_hook_id) == OK) {
                /*if (sys_irqenable(ser_hook_id) == OK) {
                        return BIT(old_hook);
                }*/
        		return BIT(old_hook);
        	}
        	return -1;
        }

        if(base==2){
        	if (sys_irqsetpolicy(SER_COM2_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, ser_hook_id) == OK) {
        	        if (sys_irqenable(ser_hook_id) == OK) {
        	        	return BIT(old_hook);
        	        }
        	}
        	        return -1;
        }
        return -1;
}

int ser_unsubscribe_int(unsigned int * ser_hook_id) {
        if (sys_irqdisable(ser_hook_id) == OK) {
                if (sys_irqrmpolicy(ser_hook_id) == OK) {
                           return 0;
                }
        }
        return -1;
}


int ser_init_fifo(int base){
	transmit_queue=new_queue();
	receive_queue=new_queue();
	printf("fifo");
	if(base==1){
		unsigned long ier;
		if(sys_inb(SER_COM1_ADDR+SER_IER, & ier)!=OK){
			return 1;
		}
		ier = (ier | ENABLE_REC_INT | ENABLE_TRANS_INT |ENABLE_LINE_STATUS_INT);
		if(sys_outb(SER_COM1_ADDR+SER_IER, ier)!=OK){
			return 1;
		}
		/*if(sys_inb(SER_COM1_ADDR+SER_FCR, &fcr)!=OK){
			return 1;
		}
		fcr = fcr | SER_EN_FIFO | SER_CLR_REC_FIFO | SER_CLR_TRANS_FIFO | SER_TRIGGER;
		if(sys_outb(SER_COM1_ADDR+SER_FCR, fcr)!=OK){
			return 1;
		}*/
		return 0;
	}
	else if(base==2){
		unsigned long fcr;//variable that will store the fcr register
		if(sys_inb(SER_COM2_ADDR+SER_FCR, &fcr)!=OK){
			return 1;
		}
		fcr = fcr | SER_EN_FIFO | SER_CLR_REC_FIFO | SER_CLR_TRANS_FIFO | SER_TRIGGER;
		if(sys_outb(SER_COM2_ADDR+SER_FCR, fcr)!=OK){
			return 1;
		}
		return 0;
	}
	return 1;
}

int ser_send_string_int(int base, unsigned long bits, unsigned long stop, long parity, unsigned long rate, char string[]){
	unsigned short base_addr;
	if(base==1){
		base_addr=SER_COM1_ADDR;
	}
	else if(base==1){
		base_addr=SER_COM2_ADDR;
	}
	else{
		return 1;
	}
	if(ser_set_bits_per_char(base_addr, bits)!=0){
		return 1;
	}

	if(ser_set_stop_bits(base_addr, stop)!=0){
		return 1;
	}

	if(ser_set_parity(base_addr, parity)!=0){
		return 1;
	}

	if(ser_set_rate(base_addr, rate)!=0){
		return 1;
	}
	//ser_init_fifo(base);
	int i=0;//counter
	do{
		push(transmit_queue, string[i]);
	}while(string[i]!=0);

    int ipc_status;//ipc_status
    message msg;//message
    int r;//variable that stores the error
    char irq_set;
    unsigned int ser_hook_id;
    if(base==1){
       	irq_set=BIT(SER_HOOK);
       	ser_hook_id=SER_HOOK;
    }
    else if(base==2){
         irq_set=BIT(SER_HOOK);
         ser_hook_id=SER_HOOK;
    }
    else{
    	return 1;
    }
    if(ser_subscribe_int(base, & ser_hook_id)==1){
       printf("Error: It wasn't possible to subscribe the interruption\n");
       return -1;
    }

    while(!empty(transmit_queue)) {
    	if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
              printf("driver_receive failed with: %d", r);
              continue;
        }

    	if (is_ipc_notify(ipc_status)) { /* received notification */

                             switch (_ENDPOINT_P(msg.m_source)) {

                             case HARDWARE: /* hardware interrupt notification */
                                     if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
                                        ser_ih(base_addr);
                                     }

                             break;

                             default:

                                     break; /* no other notifications expected: do nothing */

                             }

                             } else { /* received a standard message, not a notification */

                             /* no standard messages expected: do nothing */

                                     }

             }
            ser_unsubscribe_int(& ser_hook_id);
            return 0;

}

int ser_receive_string_int(int base,unsigned long bits, unsigned long stop, long parity, unsigned long rate){
	unsigned short base_addr;
	if(base==1){
		base_addr=SER_COM1_ADDR;
	}
	else if(base==2){
		base_addr=SER_COM2_ADDR;
	}
	else{
		return 1;
	}
	if(ser_set_bits_per_char(base_addr, bits)!=0){
		return 1;
	}

	if(ser_set_stop_bits(base_addr, stop)!=0){
		return 1;
	}

	if(ser_set_parity(base_addr, parity)!=0){
		return 1;
	}

	if(ser_set_rate(base_addr, rate)!=0){
		return 1;
	}
    int ipc_status;//ipc_status
    message msg;//message
    int r;//variable that stores the error
    char irq_set;
    unsigned int ser_hook_id;
    if(base==1){
       	irq_set=BIT(SER_HOOK);
       	ser_hook_id=SER_HOOK;
       	printf("irq");
    }
    else if(base==2){
         irq_set=BIT(SER_HOOK);
         ser_hook_id=SER_HOOK;
    }
    else{
    	return 1;
    }
    if(ser_subscribe_int(base, & ser_hook_id)==-1){
       printf("Error: It wasn't possible to subscribe the interruption\n");
       return -1;
    }
    ser_init_fifo(base);
    char c=1;
    while(c!=0) {
    	printf("b");
    	if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
              printf("driver_receive failed with: %d", r);
              continue;
        }

    	if (is_ipc_notify(ipc_status)) { /* received notification */

                             switch (_ENDPOINT_P(msg.m_source)) {

                             case HARDWARE: /* hardware interrupt notification */
                                     if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
                                    	 printf("interrupt");
                                        ser_ih(base_addr);
                                        if(!empty(receive_queue)){
                                        	c=top(receive_queue);
                                        	printf("%c",c);
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
    printf("c");
            ser_unsubscribe_int(& ser_hook_id);
            return 0;

}

int ser_ih(unsigned short base_addr){
	unsigned long iir;
	if(sys_inb(base_addr+SER_IIR, & iir)!=OK){
		return 1;
	}
	unsigned long lsr;
	if((iir & SER_INT_STATUS)==0){
		switch((iir & SER_INT_ORG)){
		case SER_INT_REC_DATA_AVLB:
			//unsigned long lsr;
			if(sys_inb(base_addr+SER_LSR, & lsr)!=OK){
				return 1;
			}
			while(lsr & SER_REC_RDY){
				unsigned long c;
				if(sys_inb(base_addr+SER_DATA, & c)!=OK){
					return 1;
				}
				push(receive_queue, c);
				if(sys_inb(base_addr+SER_LSR, & lsr)!=OK){
					return 1;
				}
			 }
			 break;
		case SER_INT_TRANS_EMPTY:
			//unsigned long lsr
			if(sys_inb(base_addr+SER_LSR, & lsr)!=OK){
				return 1;
			}
			while(lsr & SER_TX_RDY){
				if(sys_outb(base_addr+SER_DATA, top(transmit_queue))!=OK){
					return 1;
				}
				pop(transmit_queue);
				if(sys_inb(base_addr+SER_LSR, & lsr)!=OK){
					return 1;
				}
			 }
			 break;
		default:
			return 1;
		}

	}
	return 0;
}


