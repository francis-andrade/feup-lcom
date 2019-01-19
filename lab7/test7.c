#include "ser_port.h"
#include "test7.h"
int ser_test_conf(unsigned short base_addr) {
	ser_conf_t conf=get_conf(base_addr);
		printf("Bits per Char: %d\n",conf.bits_per_char);
		printf("Stop Bits: %d\n",conf.stop_bits);
		printf("Parity: %s\n",conf.parity);
		printf("Rate: %d\n",conf.rate);
		printf("Received Data Interrupt: ");
		if(conf.rec_data_intpt==1){
			printf("Enabled\n");
		}
		else{
			printf("Disabled\n");
		}
		printf("Transmitter Empty Interrupt: ");
		if(conf.trans_empty_intpt==1){
			printf("Enabled\n");
		}
		else{
			printf("Disabled\n");
		}
		printf("Receiver Line Status Interrupt: ");
		if(conf.rec_line_stat_intpt==1){
			printf("Enabled\n");
		}
		else{
			printf("Disabled\n");
		}
		return 0;
}

int ser_test_set(unsigned short base_addr, unsigned long bits, unsigned long stop, 
	           long parity, unsigned long rate) { 
	if(ser_set_bits_per_char(base_addr, bits)!=0){
		printf("Error: It wasn't possible to set the number of bits per char\n");
		return 1;
	}
	if(ser_set_stop_bits(base_addr, stop)!=0){
		printf("Error: It wasn't possible to set the number of stop bits");
		return 1;
	}
	if(ser_set_parity(base_addr, parity)!=0){
		printf("Error: It wasn't possible to set the parity\n");
		return 1;
	}
	if(ser_set_rate(base_addr, rate)!=0){
		printf("Error: It wasn't possible to set the rate\n");
		return 1;
	}
	return 0;
}


int ser_test_poll(unsigned short base_addr, unsigned char tx, unsigned long bits, 
                    unsigned long stop, long parity, unsigned long rate, 
                     char *string) {
	if(tx==1){
		printf("Sending characters poll...: %s\n",string);
			return ser_send_string_poll(base_addr, bits, stop, parity, rate, string);

	}
	else if(tx==0){
		printf("Receiving characters poll...\n");
		ser_receive_string_poll(base_addr, bits, stop, parity, rate);
	}

}

int ser_test_int(/* details to be provided */) { 
    /* To be completed */
}

int ser_test_fifo(unsigned short base, unsigned char tx, unsigned long bits,
                    unsigned long stop, long parity, unsigned long rate,
                     char *string) {
	printf("fifo test");
	if(ser_init_fifo(base)!=0){
		printf("Fifo failed");
		return 1;
	}
	if(tx==1){
		printf("Sending characters...: %s\n",string);
			return ser_send_string_poll(base, bits, stop, parity, rate, string);

	}
	else if(tx==0){
		printf("Receiving characters...\n");
		ser_receive_string_int(base, bits, stop, parity, rate);
	}
	else{
		printf("Error: tx must be 1 or 0");
	}

}
