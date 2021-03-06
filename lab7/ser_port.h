#ifndef __SER_PORT_H
#define __SER_PORT_H

#define BIT(n) (0x01<<(n))

#define SER_COM1_ADDR 0x3F8
#define SER_COM2_ADDR 0x2F8
#define SER_COM1_IRQ 4
#define SER_COM2_IRQ 3
#define SER_HOOK 5

//REGISTER
#define SER_RBR 0
#define SER_THR 0
#define SER_IER 1
#define SER_IIR 2
#define SER_FCR 2
#define SER_LCR 3
#define SER_MCR 4
#define SER_LSR 5
#define SER_MSR 6
#define SER_SR 7
#define SER_DLL 0
#define SER_DLM 1
#define SER_DLAB BIT(7)

//SER DATA
#define SER_DATA    0
#define SER_TX_RDY (1<<5)
#define SER_REC_RDY BIT(0)

//IER
#define ENABLE_REC_INT BIT(0)
#define ENABLE_TRANS_INT BIT(1)
#define ENABLE_LINE_STATUS_INT BIT(2)

//ERROR
#define SER_OVERRUN_ERR (1<<1)
#define SER_PARITY_ERR  (1<<2)
#define SER_FRAME_ERR   (1<<3)

//FIFO
#define SER_EN_FIFO BIT(0)
#define SER_CLR_REC_FIFO BIT(1)
#define SER_CLR_TRANS_FIFO BIT(2)
#define SER_TRIGGER BIT(7)

//IIR
#define SER_INT_ORG BIT(3) | BIT(2) |BIT(1)
#define SER_INT_STATUS BIT(0)
#define SER_INT_LINE_STATUS (BIT(2) |BIT(1))
#define SER_INT_TRANS_EMPTY BIT(1)
#define SER_INT_REC_DATA_AVLB BIT(2)

#define DELAY_US 20000


//struct that represents the configuration of an UART
typedef struct{
	int bits_per_char;//number of bits per char
	int stop_bits;//number of stop bits
	char parity[10];//parity
	int rate;//rate
	int rec_data_intpt;//this variable is set to 1 if this interrupt is enabled
	int trans_empty_intpt;//this variable is set to 1 if this interrupt is enabled
	int rec_line_stat_intpt;//this variable is set to 1 if this interrupt is enabled
}ser_conf_t;

/**
 * @brief This function gets the configuration of an UART register
 *
 * @param base_addr -> address of of the serial port whose configuration we want to access
 *
 * @return configuration
 */
ser_conf_t get_conf(unsigned short base_addr);
/**
 * @ brief Sets the value of bits per character in the respective serial port
 *
 * @ param base_addr -> address of the serial port whose configuration we want to change
 *
 * @ param bits -> number of bits per char that the serial port will be set to
 *
 * @ return value 0 in case of success, 1 otherwise
 *
 */
int ser_set_bits_per_char(unsigned short base_addr,int bits);
/**
 * @ brief Sets the value of bits per character in the respective serial port
 *
 * @ param base_addr -> address of the serial port whose configuration we want to change
 *
 * @ param bits -> number stop bits that the serial port will be set to
 *
 * @ return value 0 in case of success, 1 otherwise
 *
 */
int ser_set_stop_bits(unsigned short base_addr,int bits);
/**
 * @ brief Sets the value of bits per character in the respective serial port
 *
 * @ param base_addr -> address of the serial port whose configuration we want to change
 *
 * @ param parity -> parity that the serial port will be set to (its format is as specified above by the teacher)
 *
 * @ return value 0 in case of success, 1 otherwise
 *
 */
int ser_set_parity(unsigned short base_addr,int parity);
/**
 * @ brief Sets the value of bits per character in the respective serial port
 *
 * @ param base_addr -> address of the serial port whose configuration we want to change
 *
 * @ param rate rate -> that the serial port will be set to
 *
 * @ return value 0 in case of success, 1 otherwise
 *
 */
int ser_set_rate(unsigned short base_addr,int rate);
/**
 * @brief Sets the configuration of the serial port
 *
 * @param base_addr -> address of the serial port whose configuration we want to change
 *
 * @param parity -> parity that the serial port will be set to (its format is as specified above by the teacher)
 *
 * @param rate -> rate that the serial port will be set to
 *
 * @return value 0 in case of success, 1 otherwise
 *
 */
int ser_set_conf(unsigned short base_addr, int bits, int stop, int parity, int rate);
/**
 * @brief Checks if the lsr register has an error
 *
 * @param base_addr -> address of the serial port whose configuration we want to change
 *
 * @return value 0 if there is no error, 1 if there and -1 if there was a problem obtaining the lsr error
 *
 */
int ser_check_lsr_error(unsigned short base_addr);
/**
 * @brief Disables IER interrupts
 *
 * @ param base_addr -> address of the serial port whose configuration we want to change
 *
 * @ return 0 in case of success, 1 otherwise
 *
 */
int ser_disable_int(unsigned short base_addr);
//POLL
/**
 * @brief transmits a char through the serial port
 * @param base_addr -> address of the serial port we want to use to change the character
 * @param c -> character that we want to transmit
 * @param num_cicles -> maximum number of cicles that the function will execute
 * @return 0 in case of success, 1 otherwise
 */
int ser_send_char_poll(unsigned short base_addr, char c, int num_cicles);
/**
 * @brief receive a char through the serial port
 * @param base_addr -> address of the serial port we want to use to change the character
 * @param c -> pointer to the variable that is going to store the character
 * @param num_cicles -> maximum nnumber of cicles that the function will execute
 * @return 0 in case of success, 1 otherwise
 */
int ser_receive_char_poll(unsigned short base_addr, char * c, int num_cicles);

int ser_send_string_poll(unsigned short base_addr,unsigned long bits, unsigned long stop, long parity, unsigned long rate, char string[]);

int ser_receive_string_poll(unsigned short base_addr,unsigned long bits, unsigned long stop, long parity, unsigned long rate);

int ser_send_strings_poll(unsigned short base_addr, unsigned long bits, unsigned long stop, long parity, unsigned long rate, int stringc, char *strings[]);
/**
 * @brief subscribes a serial port interruption
 * @param base 1->COM1; 2->COM2
 * @param ser_hook_id serial port hook
 * @return Return -1 in case of success or positive value otherwise
 */
int ser_subscribe_int(unsigned int base, unsigned int * ser_hook_id);
/**
 * @brief unsubscribes a serial port interruption
 * @param ser_hook_id serial port hook
 * @return Return 0 upon success and non-zero otherwise
 */
int ser_unsubscribe_int(unsigned int * ser_hook_id);
/**
 * @brief inititalizes the fifo queues and the fifo register
 * @param base 1->COM1; 2->COM2
 * @return Return 0 upon success and non-zero otherwise
 */
int ser_init_fifo(int base);

int ser_ih(unsigned short base_addr);

int ser_receive_string_int(int base,unsigned long bits, unsigned long stop, long parity, unsigned long rate);

int ser_send_string_int(int base, unsigned long bits, unsigned long stop, long parity, unsigned long rate, char string[]);
#endif
