#include "test3.h"
#include "i8042.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>

static int proc_args(int argc, char **argv);
static unsigned short parse_ushort(char *str, int base);
static int parse_array(char *str, unsigned short n, unsigned short * leds);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup();
	/* DO NOT FORGET TO initialize service */
	 sys_enable_iop(SELF);

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"scan <0/1 - c/assembly>\"\n"
			"\t service run %s -args \"leds <decimal no. -n> <sequence with size n 0/1/2 - toggle>\"\n"
			"\t service run %s -args \"timed <decimal no. - time interval>\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short ass, n;
	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if (argc != 3) {
			printf("test3: wrong no. of arguments for kbd_test_scan()\n");
			return 1;
		}
		ass = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (ass == USHRT_MAX)
			return 1;
		printf("test3::kbd_test_scan(%u)\n", ass);
		return kbd_test_scan(ass);
	}
	else if (strncmp(argv[1], "leds", strlen("leds")) == 0) {
		if (argc != 4) {
			printf("test3: wrong no. of arguments for kbd_test_leds()\n");
			return 1;
		}
		n = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (n == USHRT_MAX)
			return 1;
		if(strlen(argv[3])!=n){
			printf("test3: wrong size of toggle sequence for kbd_test_leds()\n");
			return 1;
		}
		unsigned short * leds=(unsigned short *) malloc(n*sizeof(unsigned short));
		if(parse_array(argv[3],n, leds)!=0){
			printf("test3: wrong toggle sequence for kbd_test_leds()\n");
			return 1;
		}
		printf("test3::kbd_test_leds(%u, %s)\n", n, argv[3]);
		return kbd_test_leds(n, leds);
	}
	else if (strncmp(argv[1], "timed", strlen("timed")) == 0) {
		if (argc != 3) {
			printf("test3: wrong no of arguments for kbd_test_timed_scan()\n");
			return 1;
		}
		n = parse_ushort(argv[2], 10);						/* Parses string to unsigned long */
		if (n == USHRT_MAX)
			return 1;
		printf("test3::kbd_test_timed_scan(%u)\n", n);
		return kbd_test_timed_scan(n);
	}
	else {
		printf("test3: %s - no valid function!\n", argv[1]);
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
