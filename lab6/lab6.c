#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>
#include "test6.h"

static int proc_args(int argc, char **argv);
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
			"\t service run %s -args \"conf\"\n"
			"\t service run %s -args \"date\"\n"
			"\t service run %s -args \"int \"\n"
			, argv[0], argv[0],argv[0]);
}

static int proc_args(int argc, char **argv)
{

	if (strncmp(argv[1], "conf", strlen("conf")) == 0) {
		if (argc != 2) {
			printf("test6: wrong no. of arguments for rtc_test_conf()\n");
			return 1;
		}

		printf("test6::rtc_test_conf()\n");
		return rtc_test_conf();
	}
	else if (strncmp(argv[1], "date", strlen("date")) == 0) {
		if (argc != 2) {
			printf("test6: wrong no. of arguments for rtc_test_date()\n");
			return 1;
		}

		printf("test6::rtc_test_date()\n");
		return rtc_test_date();
	}
	else if (strncmp(argv[1], "int", strlen("int")) == 0) {
		if (argc != 2) {
			printf("test6: wrong no. of arguments for rtc_test_int()\n");
			return 1;
		}
		printf("test6::rtc_test_int()\n");
		return rtc_test_int();
	}
	else {
		printf("test6: %s - no valid function!\n", argv[1]);
		return 1;
	}
}



