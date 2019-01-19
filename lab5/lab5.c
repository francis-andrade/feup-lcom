#include "test5.h"
#include "i8042.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>
#include "pixmap.h"

static int proc_args(int argc, char **argv);
static unsigned short parse_ushort(char *str, int base);
static unsigned short ignore_parse_ushort(char *str, int base);
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
			"\t service run %s -args \"init <decimal no. - mode> <decimal no. delay>\"\n"
			"\t service run %s -args \"square <decimal no. -x> <decimal no. -y> <decimal no. -size> <decimal no. -color> \"\n"
			"\t service run %s -args \"line <decimal no. -xi> <decimal no. -yi>  <decimal no. -xf> <decimal no. -yf> <decimal no. -color>\"\n"
			"\t service run %s -args \"xpm <decimal no. -xi> <decimal no. -yi> <decimal no. xpm_id>\"\n "
			"\t service run %s -args \"move <decimal no. -xi> <decimal no. -yi> <decimal no. xpm_id> <decimal no. hor> <decimal no. delta> <decimal no. time>\"\n "
			"\t service run %s -args \"controller\"\n "
			,argv[0],argv[0],argv[0], argv[0], argv[0],argv[0]);
}

static int proc_args(int argc, char **argv)
{

	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		unsigned short mode,delay;
		if (argc != 4) {
			printf("test5: wrong no. of arguments for test_init()\n");
			return 1;
		}
		mode = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		delay=parse_ushort(argv[3],10);
		printf("test5::test_init(%d, %d)\n", mode, delay);
		test_init(mode, delay);
		return 0;
	}
	else if (strncmp(argv[1], "square", strlen("square")) == 0) {
		unsigned short x, y, size, color;
		if (argc != 6) {
			printf("test5: wrong no. of arguments for test_square()\n");
			return 1;
		}
		x = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		y = parse_ushort(argv[3], 10);
		size = parse_ushort(argv[4], 10);
		color = parse_ushort(argv[5], 10);

		printf("test5::test_square(%d, %d, %d, %d)\n", x, y, size, color);
		return test_square(x,y,size,color);
	}
	else if (strncmp(argv[1], "line", strlen("line")) == 0) {
		unsigned short xi, yi, xf, yf, color;
		if (argc != 7) {
			printf("test5: wrong no. of arguments for test_line()\n");
			return 1;
		}
		xi = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		yi = parse_ushort(argv[3], 10);
		xf = parse_ushort(argv[4], 10);
		yf = parse_ushort(argv[5], 10);
		color=parse_ushort(argv[6],10);
		printf("test5::test_line(%d, %d, %d, %d, %d)\n",xi,yi,xf,yf,color);
		return test_line(xi,yi,xf,yf,color);
	}
	else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0) {
		unsigned short xi, yi, xpm;
		if (argc != 5) {
			printf("test5: wrong no. of arguments for test_xpm()\n");
			return 1;
		}
		xi = parse_ushort(argv[2], 10);
		yi= parse_ushort(argv[3], 10);
		xpm=parse_ushort(argv[4], 10);
		if(xpm>4){
			printf("test5: that xpm is not available1n");
			return 1;
		}
		printf("test5:test_xpm(%d, %d, %d)\n", xi, yi, xpm);
		return test_xpm(xi,yi,get_xpm(xpm));
	}
	else if (strncmp(argv[1], "move", strlen("move")) == 0) {
			unsigned short xi, yi, xpm,hor,delta,time;
			if (argc != 8) {
				printf("test5: wrong no. of arguments for test_move()\n");
				return 1;
			}
			xi = parse_ushort(argv[2], 10);
			yi= parse_ushort(argv[3], 10);
			xpm=parse_ushort(argv[4], 10);
			hor = parse_ushort(argv[5], 10);
			if(argv[6][0]=='-'){
				char * c=argv[6];
				c++;
				delta=-parse_ushort(c,10);
			}
			else{
				delta=parse_ushort(argv[6],10);
			}
			time=parse_ushort(argv[7], 10);
			if(xpm>4){
				printf("test5: That xpm is not available\n");
				return 1;
			}


			printf("test5:test_move(%d, %d, %d, %d, %d, %d)\n", xi, yi, xpm, hor, delta, time);
			return test_move(xi, yi, get_xpm(xpm),hor, delta, time);
		}
	else if (strncmp(argv[1], "controller", strlen("controller")) == 0) {
				if (argc != 2) {
					printf("test5: wrong no. of arguments for test_move()\n");
					return 1;
				}

				printf("test5::test_gcontroller()\n");
				return test_controller();
			}
	else {
		printf("test5: %s - no valid function!\n", argv[1]);
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

static unsigned short ignore_parse_ushort(char *str, int base){

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


