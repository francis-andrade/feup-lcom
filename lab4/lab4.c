#include "test4.h"
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


	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"packet <decimal no. - cnt>\"\n"
			"\t service run %s -args \"async <decimal no. -idle_time> \"\n"
			"\t service run %s -args \"config \"\n"
			"\t service run %s -args \"gesture <decimal no. -length>\"\n "
			,argv[0], argv[0], argv[0],argv[0]);
}

static int proc_args(int argc, char **argv)
{

	if (strncmp(argv[1], "packet", strlen("scan")) == 0) {
		unsigned short cnt;
		if (argc != 3) {
			printf("test4: wrong no. of arguments for test_packet()\n");
			return 1;
		}
		cnt = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (cnt == USHRT_MAX)
			return 1;
		printf("test4::test_packet(%d)\n", cnt);
		return test_packet(cnt);
	}
	else if (strncmp(argv[1], "async", strlen("async")) == 0) {
		unsigned short idle_time;
		if (argc != 3) {
			printf("test4: wrong no. of arguments for test_async()\n");
			return 1;
		}
		idle_time = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (idle_time == USHRT_MAX)
			return 1;
		printf("test4::test_async(%d)\n", idle_time);
		return test_async(idle_time);
	}
	else if (strncmp(argv[1], "config", strlen("config")) == 0) {
		if (argc != 2) {
			printf("test4: wrong no. of arguments for test_config()\n");
			return 1;
		}
		printf("test4::test_config()\n");
		return test_config();
	}
	else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
		short length;
		if (argc != 3) {
			printf("test4: wrong no. of arguments for test_gesture()\n");
			return 1;
		}
		length = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (length == SHRT_MAX)
			return 1;
		printf("test4::test_gesture(%d)\n", length);
		return test_gesture(length);
	}
	else {
		printf("test4: %s - no valid function!\n", argv[1]);
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
