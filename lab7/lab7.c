#include "test7.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>

static int proc_args(int argc, char **argv);
static unsigned short parse_ushort(char *str, int base);
static int parse_array(char *str, unsigned short n, unsigned short * leds);
static void print_usage(char **argv);
static unsigned int parse_uint(char *str, int base);

int main(int argc, char **argv)
{
	sef_startup();
	/* DO NOT FORGET TO initialize service */

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"conf <decimal no. - base_addr>\"\n"
			"\t service run %s -args \"set <decimal no. -base_addr> <decimal no. bits> <decimal no. stop> <-1/0/1 parity> <decimal no. rate>\"\n"
			"\t service run %s -args \"poll <0/1 - tx> <decimal no. -base_addr> <decimal no. bits> <decimal no. stop> <-1/0/1 parity> <decimal no. rate> <string>\"\n"
			"\t service run %s -args \"fifo <0/1 - tx> <decimal no. -base> <decimal no. bits> <decimal no. stop> <-1/0/1 parity> <decimal no. rate> <string>\"\n"
			,argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{

	if (strncmp(argv[1], "conf", strlen("conf")) == 0) {
		unsigned short base_addr;
		if (argc != 3) {
			printf("test7: wrong no. of arguments for ser_test_conf()\n");
			return 1;
		}
		base_addr = parse_ushort(argv[2], 16);						/* Parses string to unsigned short */
		if (base_addr == USHRT_MAX)
			return 1;
		printf("test7::ser_test_conf(%d)\n",base_addr);
		return ser_test_conf(base_addr);
	}
	else if (strncmp(argv[1], "set", strlen("set")) == 0) {
		unsigned short base_addr, bits, stop, rate;
		int parity;
		if (argc != 7) {
			printf("test7:: wrong no. of arguments for ser_test_set()\n");
			return 1;
		}
		base_addr = parse_ushort(argv[2], 16);						/* Parses string to unsigned short */
		if (base_addr == USHRT_MAX)
			return 1;
		bits = parse_ushort(argv[3], 10);						/* Parses string to unsigned short */
		if (bits == USHRT_MAX)
			return 1;
		stop = parse_ushort(argv[4], 10);						/* Parses string to unsigned short */
		if (stop== USHRT_MAX)
			return 1;
		printf("%c",argv[5][0]);
		if(argv[5][0]=='-'){
			parity=-1;
		}
		else if(argv[5][0]=='0'){
			parity=0;
		}
		else{
			parity=1;
		}
		printf("rate string: %s ",argv[6]);
		rate = parse_uint(argv[6], 10);						/* Parses string to unsigned short */
		printf("rate value: %d",rate);
		printf("test7::ser_test_set(%d, %d, %d, %d, %d)\n", base_addr, bits, stop, parity, rate);
		return ser_test_set(base_addr, bits, stop, parity, rate);
	}
	else if (strncmp(argv[1], "poll", strlen("poll")) == 0) {
			char tx;
			int parity;
			unsigned short base_addr, bits, stop, rate;
			if (argc !=9) {
				printf("test7:: wrong no. of arguments for ser_test_poll()\n");
				return 1;
			}
			tx=parse_ushort(argv[2],10);
			if(tx!=0 && tx!=1){
				printf("error: wrong tx parameter");
				return 1;
			}
			base_addr = parse_ushort(argv[3], 16);						/* Parses string to unsigned short */
			if (base_addr == USHRT_MAX)
				return 1;
			bits = parse_ushort(argv[4], 10);						/* Parses string to unsigned short */
			if (bits == USHRT_MAX)
				return 1;
			stop = parse_ushort(argv[5], 10);						/* Parses string to unsigned short */
			if (stop== USHRT_MAX)
				return 1;

			if(argv[6][0]=='-'){
				parity=-1;
			}
			else if(argv[6][0]=='0'){
				parity=0;
			}
			else{
				parity=1;
			}
			rate = parse_uint(argv[7], 10);						/* Parses string to unsigned short */

			printf("test7::ser_test_poll(%x, %d, %d, %d, %d, %s)\n", base_addr, bits, stop, parity, rate, argv[8]);
			return ser_test_poll(base_addr,tx, bits, stop, parity, rate,argv[8]);
		}
	else if (strncmp(argv[1], "fifo", strlen("fifo")) == 0) {
		printf("a");
				char tx;
				int parity;
				unsigned short base, bits, stop, rate;
				if (argc !=9) {
					printf("test7:: wrong no. of arguments for ser_test_fifo()\n");
					return 1;
				}
				tx=parse_ushort(argv[2],10);
				if(tx!=0 && tx!=1){
					printf("error: wrong tx parameter");
					return 1;
				}
				printf("a");
				base = parse_ushort(argv[3], 10);						/* Parses string to unsigned short */
				if (base == USHRT_MAX)
					return 1;
				printf("a");
				bits = parse_ushort(argv[4], 10);						/* Parses string to unsigned short */
				if (bits == USHRT_MAX)
					return 1;
				printf("a");
				stop = parse_ushort(argv[5], 10);						/* Parses string to unsigned short */
				if (stop== USHRT_MAX)
					return 1;
				printf("a");
				if(argv[6][0]=='-'){
					parity=-1;
				}
				else if(argv[6][0]=='0'){
					parity=0;
				}
				else{
					parity=1;
				}
				printf("a");
				rate = parse_ushort(argv[7], 10);						/* Parses string to unsigned short */
				if (rate== USHRT_MAX)
					return 1;
				printf("a");
				printf("test7::ser_test_fifo(%d, %d, %d, %d, %d, %d, %s)\n", base,tx, bits, stop, parity, rate, argv[8]);
				return ser_test_fifo(base,tx, bits, stop, parity, rate,argv[8]);
	}
	else {
		printf("test7: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned short parse_ushort(char *str, int base)
{
	char *endptr;
	unsigned short val;

	/* Convert string to unsigned short */
	val = strtol(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == USHRT_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return USHRT_MAX;
	}

	if (endptr == str) {
		printf("test3c: parse_ushort: no digits were found in %s\n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}

static unsigned int parse_uint(char *str, int base)
{
	char *endptr;
	unsigned int val;

	/* Convert string to unsigned short */
	val = strtol(str, &endptr, base);


	if (endptr == str) {
		printf("test3c: parse_ushort: no digits were found in %s\n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}

static int parse_array(char *str, unsigned short n, unsigned short * leds){
	unsigned int i;
	for(i=0;i<n;i++){
		if(str[i]<48 || str[i]>50){
			return 1;
		}
		else{
			leds[i]=str[i]-48;
		}
	}
	return 0;
}
