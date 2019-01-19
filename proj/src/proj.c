#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>
#include "checkers.h"
#include "utilities.h"
#include "proj.h"
#include "ser_port.h"

static void print_usage(char **argv);
static int proc_args(int argc, char **argv);
static void ret_path_images(char * arg, char * path_images);


int main(int argc, char **argv)
{
	sef_startup();
	sys_enable_iop(SELF);
	if(argc==1){
		print_usage(argv);
		return 0;
	}
	else{
		return proc_args(argc, argv);
	}
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"normal\"\n"
			"\t service run %s -args \"serial <1|2 player> \"\n"
			,argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{

	if (strncmp(argv[1], "normal", strlen("normal"))==0) {
		if(argc != 2){
			printf("proj Error: Wrong number of arguments for normal mode\n");
			return 1;
		}
		char * path_images=malloc(256*sizeof(char));
		ret_path_images(argv[0], path_images);
		int r=checkers_start_game(path_images, 1, 1);
		free(path_images);
		return r;
	}
	else if (strncmp(argv[1], "serial", strlen("serial")) == 0) {
		if(argc !=3){
			printf("proj Error: Wrong number of arguments for serial mode\n");
			return 1;
		}
		if(strcmp(argv[2], "1")==0){
			return connect_serial(1,argv[0]);
		}
		else if(strcmp(argv[2], "2")==0){
			return connect_serial(2, argv[0]);
		}
		else{
			printf("proj Error: In serial mode, the argument player can only be 1 or 2\n");
		}
	}
	else {
		printf("test7: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static void ret_path_images(char * arg, char * path_images){
	char string[256];
	strcpy(string,arg);
	memcpy(path_images, string, strlen(string)-strlen("proj"));
	path_images[strlen(string)-strlen(string)-strlen("proj")]='\0';
	strcat(path_images, "images/");
}

int connect_serial(char player, char * arg0){
	if(player==1){
		if(ser_disable_int(PORT_ADDR)!=0){
			printf("Proj Error: Could not use the serial port\n");
			return 1;
		}
		if(ser_set_conf(PORT_ADDR, PORT_BITS, PORT_STOP, PORT_PARITY, PORT_RATE)!=0){
			printf("Proj Error: Could not use the serial port\n");
			return 1;
		}
		printf("Waiting for player 2...\n");
		unsigned long c;//character that is going to be received by player 1
		if(ser_receive_char_poll(PORT_ADDR,& c, 10000)!=0){
			printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
			return 1;
		}
		else{
			if(c!='b'){
				printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
				return 1;
			}
		}
		if(ser_receive_char_poll(PORT_ADDR,& c, 10000)!=0){
			printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
			return 1;
		}
		else{
			if(c!=2){
				printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
				return 1;
			}
		}
		if(ser_receive_char_poll(PORT_ADDR,& c, 10000)!=0){
				printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
				return 1;
		}
		else{
			if(c!=0){
				printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
				return 1;
			}
		}
		if(ser_send_char_poll(PORT_ADDR, 'a',0)!=0){
			printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
			return 1;
		}
		if(ser_send_char_poll(PORT_ADDR, 1, 0)!=0){
			printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
			return 1;
		}
		if(ser_send_char_poll(PORT_ADDR, 0, 0)!=0){
			printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
			return 1;
		}
		printf("Connection successful Player 1\n");
		char * path_images=malloc(256*sizeof(char));
		ret_path_images( arg0, path_images);
		int r= checkers_start_game(path_images, 1, 0);
		free(path_images);
		return r;

	}
	else if(player==2){
		if(ser_disable_int(PORT_ADDR)!=0){
			printf("Proj Error: Could not use the serial port\n");
			return 1;
		}
		if(ser_set_conf(PORT_ADDR, PORT_BITS, PORT_STOP, PORT_PARITY, PORT_RATE)!=0){
			printf("Proj Error: Could not use the serial port\n");
			return 1;
		}
		if(ser_send_char_poll(PORT_ADDR, 'b',0)){
			printf("Proj Error: Could not connect to player 1\n Make sure player 1 runs the program BEFORE you.\n");
			return 1;
		}
		if(ser_send_char_poll(PORT_ADDR, 2, 0)!=0){
			printf("Proj Error: Could not connect to player 1\n Make sure player 1 runs the program BEFORE you.\n");
			return 1;
		}
		if(ser_send_char_poll(PORT_ADDR, 0, 0)!=0){
			printf("Proj Error: Could not connect to player 1\n Make sure player 1 runs the program BEFORE you.\n");
			return 1;
		}
		unsigned long c;//character that is going to be received by player 1
		if(ser_receive_char_poll(PORT_ADDR,& c, 10)!=0){
			printf("Proj Error: Could not connect to player 1\n Make sure player 1 runs the program BEFORE you.\n");
			return 1;
		}
		else{
			if(c!='a'){
				printf("Proj Error: Could not connect to player 1\n Make sure player 1 runs the program BEFORE you.\n");
				return 1;
			}
		}
			if(ser_receive_char_poll(PORT_ADDR,& c, 10)!=0){
				printf("Proj Error: Could not connect to player 1\n Make sure player 1 runs the program BEFORE you.\n");
				return 1;
			}
			else{
				if(c!=1){
					printf("Proj Error: Could not connect to player 1\n Make sure player 1 runs the program BEFORE you.\n");
					return 1;
				}
			}
			if(ser_receive_char_poll(PORT_ADDR,& c, 10)!=0){
				printf("Proj Error: Could not connect to player 1\n Make sure player 1 runs the program BEFORE you.\n");
					return 1;
			}
			else{
				if(c!=0){
					printf("Proj Error: Could not connect to player 1\n Make sure player 1 runs the program BEFORE you.\n");
					return 1;
				}
			}

		printf("Connection successful player 2\n");
		char * path_images=malloc(256*sizeof(char));
		ret_path_images(arg0, path_images);
		int r=checkers_start_game(path_images, 0, 1);
		return r;
	}
}
