#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "ser_port.h"
/*#include "queue.h"

static queue_t * transmit_queue;
static queue_t * receive_queue;*/

ser_conf_t get_conf(unsigned short base_addr){
	ser_conf_t r;//configuration that is going to be returned
	unsigned long lcr;//variable that is going to store the lcr register
	if(sys_inb(base_addr+SER_LCR, & lcr)!=OK){
		return r;
	}
	r.bits_per_char=5+(lcr & (BIT(0) | BIT(1)));
	//printf("lcr: %x\n",lcr);
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

int ser_receive_char_poll(unsigned short base_addr, unsigned long * c, int num_cicles){
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
 if(sys_inb(base_addr+SER_DATA, c)!=OK){
	 return 1;
 }
printf("Received char: %d\n",* c);
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
 printf("Sent char: %d\n",c);
 return 0;
}





