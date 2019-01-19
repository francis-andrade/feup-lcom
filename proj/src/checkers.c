#include <errno.h>
#include <minix/drivers.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "checkers.h"
#include "ProgramLogic.h"
#include "video_gr.h"
#include "vbe.h"
#include "i8042.h"
#include "i8254.h"
#include "utilities.h"
#include "mouse.h"
#include "timer.h"
#include "Bitmap.h"
#include "kbc.h"
#include "proj.h"
#include "publicity.h"

static int state=STATE_1_DEFAULT;
static char board[8][8];
static mouse_t * mouse;

static Bitmap * cursor;
static Bitmap * redturn;
static Bitmap * yellowturn;
static Bitmap * background;




int checkers_start_game(char * path_images, int player_1, int player_2){
	if(player_1==1 && player_2==1){
		if(show_ad(path_images)!=0){
			printf("Error: It wasn't possible to load the ads\n");
			return 1;
		}
	}
	timer_set_square(0, 60);
	char * path=malloc(sizeof(char)*256);
	strcpy(path, path_images);
	strcat(path, "cursor.bmp");
	cursor=loadBitmap(path);
	strcpy(path, path_images);
	strcat(path, "redturn.bmp");
	redturn=loadBitmap(path);
	strcpy(path, path_images);
	strcat(path, "yellowturn.bmp");
	yellowturn=loadBitmap(path);
	strcpy(path, path_images);
	strcat(path, "background.bmp");
	background=loadBitmap(path);
    free(path);
	createBoard(board);
	if(vg_init(VBE_565_MODE)==NULL){
		vg_exit();
		printf("It wasn't possible to initiate the 565 Mode\n");
		return 1;
	}
	if(create_mouse(200,200)!=0){
		vg_exit();
		return 1;
	}
	drawBitmap(background,0,0,ALIGN_LEFT);
	draw_board();
	mouse->bitmap=cursor;
	draw_turn_message(TURN_MSG_XI_YELLOW, TURN_MSG_YI,TURN_MSG_YELLOW_BACKGROUND_CLR, yellowturn);
	draw_mouse();
	swap_mouse_video();
	int r;
	if(player_1 !=1 || player_2 !=1){
		printf("SERIAL");
		r=checkers_serial_handler(player_1, player_2);
		printf("r final: %d",r);
	}
	else{
		printf("NORMAL\n");
		r=checkers_int_handler(player_1, player_2);
		printf("r final: %d\n",r);
	}
	//delete_mouse();
	vg_exit();
	if(r==0){
		printf("\nexit");
	}
	else if(r==1){
		printf("\nyellows won due to timeout");
	}
	else if(r==2){
		printf("\nreds won due to timeout");
	}
	else if(r==3){
		printf("\nyellows won the game");
	}
	else if(r==4){
		printf("\nreds won the game");
	}
	else{
		printf("\nError");
	}
}


int checkers_serial_handler(int player1, int player2){
	char player[3];
	player[1]=player1;
	player[2]=player2;
	char gameover=0;//this variable will be set to 1 when the game ends
	while(1){
		if(player1==1 && state_to_player(state)==1){
			int r=checkers_int_handler(player1, player2);
			if(r!=5){
				return r;
			}
		}
		else if(player1==1 && state_to_player(state)==2){
			int r, xi, yi, xf, yf;
			if(wait_for_serial_message(1, & r, & xi, & yi, & xf, & yf)!=0){
				printf("Error : Couldn't connect to player 2\n");
				return -1;
			}
			if(r==5 || r==3 || r==4){
				move_piece(2, xi, yi, xf, yf);
				state=STATE_1_DEFAULT;
				draw_turn_message(TURN_MSG_XI_YELLOW, TURN_MSG_YI,TURN_MSG_YELLOW_BACKGROUND_CLR, yellowturn);
				if(r==3 || r==4){
					return r;
				}
			}
			else{
				return r;
			}
		}
		if(player2==1 && state_to_player(state)==2){
			int r=checkers_int_handler(player1, player2);
			if(r!=5){
				return r;
			}
		}
		else if(player2==1 && state_to_player(state)==1){
			int r, xi, yi, xf, yf;
			if(wait_for_serial_message(2, & r, & xi, & yi, & xf, & yf)!=0){
				printf("Error : Couldn't connect to player 1\n");
				return -1;
			}
			if(r==5 || r==3 || r==4){
				move_piece(1, xi, yi, xf, yf);
				state=STATE_2_DEFAULT;
				draw_turn_message(TURN_MSG_XI_RED, TURN_MSG_YI,TURN_MSG_RED_BACKGROUND_CLR, redturn);
				if(r==3 || r==4){
					return r;
				}
			}
			else{
				return r;
			}
		}

	}
}

int checkers_int_handler(int player1,int player2){

	char serial;
	if(player1 != 1 || player2 !=1){
		serial=1;
	}
	else{
		serial=0;
	}
	int ipc_status; //ipc_status
	message msg; //message
	int r; //variable that stores the error
	long irq_set_timer=BIT(timer_hook_id);
	int kbd_hook_id = KBC_IRQ; //kbd_hook
	long irq_set_kbd = BIT(kbd_hook_id);
	int mouse_hook_id=MOUSE_IRQ;//mouse hook
	long irq_set_mouse=BIT(mouse_hook_id);
    if (timer_subscribe_int() == -1) {
            return -1;

    }

	if (kbd_subscribe_int(&kbd_hook_id) == -1) {
		return -1;
	}
	if(mouse_subscribe_int(& mouse_hook_id) == -1){
		return -1;
	}

	if(mouse_cmd_write(MOUSE_SET_STREAM)!=0){
		return -1;
	}

	if(mouse_cmd_write(MOUSE_ENABLE)!=0){
		return -1;
	}
	ignore_interrupts(MOUSE_IRQ);//ignores pevious interrupts


	unsigned long scancode = 0; //scancode of the key that the user is going to press
	int counter=0;//counts the time
    unsigned long next_byte=0;//next byte to be read from the mouse packet
    unsigned char packet[3];
	char change_state=0;//this variable works like a bool and will be set to 1 when the state is changed
	int bol=0;
	while (scancode != 0x81 && !isEndGame(board) && counter< CHECKERS_TIMEOUT*  60 && !(change_state & serial) && !bol) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if(msg.NOTIFY_ARG & irq_set_mouse){
					int current_byte=next_byte;
					mouse_get_packet(& next_byte, packet);
					mouse_packet_t mouse_packet;
					if(current_byte==2 && next_byte==0){
						mouse_convert_packet(packet, & mouse_packet);
						int old_state=state;
						if(checkers_move_handler(mouse_packet,player1, player2)!=0){
							bol=1;
						}
						if(state_to_player(state)!=state_to_player(old_state)){
							change_state=1;
							printf("state changed\n");
							counter=0;
						}
					}

				}
				if (msg.NOTIFY_ARG & irq_set_kbd) {
						kbc_c_to_ass(& scancode);
				}
				if(msg.NOTIFY_ARG & irq_set_timer){
						counter++;
				}
				break;
			default:
				break;
			}
		}
	}
	printf("UNSUBSCRIBE\n");
	if (mouse_unsubscribe_int(&kbd_hook_id)!=0) {
		return -1;
	}

    if (mouse_cmd_write(MOUSE_DISABLE_STREAM_MODE) != 0) {
        return -1;
    }

	if (kbd_unsubscribe_int(&kbd_hook_id)!=0) {
		//return -1;
	}

    if (timer_unsubscribe_int() != 0) {
            return -1;
    }
    if(bol){
    	printf("Checkers int handler fail: bol is not correct\n");
    	return -1;
    }
	if(scancode==0x81){
		if(serial){
			send_serial_ret(0);
		}
		return 0;
	}
	else if(counter>=CHECKERS_TIMEOUT*60 && (state==STATE_2_DEFAULT || state==STATE_2_HOLD_PIECE)){
		if(serial){
			send_serial_ret(1);
		}
		return 1;
	}
	else if(counter>=CHECKERS_TIMEOUT*60 && (state==STATE_1_DEFAULT || state==STATE_1_HOLD_PIECE)){
		if(serial){
			send_serial_ret(2);
		}
		return 2;
	}
	else if(isEndGame(board) && state==STATE_2_DEFAULT){
		if(serial){
			send_serial_ret(3);
		}
		return 3;
	}
	else if(isEndGame(board) && state==STATE_1_DEFAULT){
		if(serial){
			send_serial_ret(4);
		}
		return 4;
	}
	else if(change_state==1){
		if(serial){
			send_serial_ret(5);
		}
		return 5;
	}
	else{
		return -1;
	}
}

int checkers_move_handler(mouse_packet_t packet, int player1,int player2){
	char serial;
	if(player1 != 1 || player2 !=1){
		serial=1;
	}
	else{
		serial=0;
	}
	switch(state){
	case STATE_1_DEFAULT:
		if(packet.lb==0){
			move_mouse(packet.x, packet.y);
			swap_mouse_video();
		}
		else{
			int board_x;
			int board_y;
			coord_to_real_board(mouse->x+packet.x, mouse->y+packet.y, & board_x, & board_y);
			if(board[board_x][board_y]==1){
				int real_x, real_y;
				coord_to_real_board(mouse->x+packet.x, mouse->y-packet.y, & real_x, & real_y);
				mouse->hold=1;
				mouse->radius=BOARD_CL_RADIUS;
				mouse->color_circle=BOARD_PL1_CL_CLR;
				mouse->x_circle=mouse->x+packet.x-(BOARD_XI+real_x*BOARD_SQ_SIZE+BOARD_SQ_SIZE/2);
				mouse->y_circle=mouse->y-packet.y-(BOARD_YI+real_y*BOARD_SQ_SIZE+BOARD_SQ_SIZE/2);
				mouse->original_x=real_x;
				mouse->original_y=real_y;
				put_square(real_x, real_y, BOARD_ODD_SQ_CLR);
				state=STATE_1_HOLD_PIECE;
			}
			move_mouse(packet.x, packet.y);
			swap_mouse_video();
		}
		break;
	case STATE_1_HOLD_PIECE:
		if(packet.lb==0){
			mouse->hold=0;
			swap_double_mouse();
			int board_x, board_y;
			coord_to_real_board(mouse->x+packet.x, mouse->y-packet.y, & board_x, & board_y);
			int xi=mouse->original_x;
			int yi=mouse->original_y;
			int r=moveDisc(board, 1, xi, yi, board_x, board_y);
			if(r==0 || r==5 || r==6){
				put_circle(mouse->original_x, mouse->original_y, BOARD_PL1_CL_CLR);
				state=STATE_1_DEFAULT;
			}
			else if(r==1 || r==2){
				put_circle(board_x, board_y, BOARD_PL1_CL_CLR);
				state=STATE_2_DEFAULT;
				if(serial==1){
					if(send_serial_move(1, mouse->original_x, mouse->original_y, board_x, board_y)!=0){
						return 1;
					}
				}
				draw_turn_message(TURN_MSG_XI_RED, TURN_MSG_YI,TURN_MSG_RED_BACKGROUND_CLR, redturn);
			}
			else if(r==3){
				put_square(board_x-1, board_y+1, BOARD_ODD_SQ_CLR);
				put_circle(board_x, board_y, BOARD_PL1_CL_CLR);
				state=STATE_2_DEFAULT;
				if(serial==1){
					if(send_serial_move(1, mouse->original_x, mouse->original_y, board_x, board_y)!=0){
						return 1;
					}
				}
				draw_turn_message(TURN_MSG_XI_RED, TURN_MSG_YI,TURN_MSG_RED_BACKGROUND_CLR, redturn);
			}
			else if(r==4){
				put_square(board_x+1, board_y+1,  BOARD_ODD_SQ_CLR);
				put_circle(board_x, board_y, BOARD_PL1_CL_CLR);
				state=STATE_2_DEFAULT;
				if(serial==1){
					if(send_serial_move(1, mouse->original_x, mouse->original_y, board_x, board_y)!=0){
						return 1;
					}
				}
				draw_turn_message(TURN_MSG_XI_RED, TURN_MSG_YI,TURN_MSG_RED_BACKGROUND_CLR, redturn);
			}
			move_mouse(packet.x,packet.y);
			swap_mouse_video();
		}
		else{
			move_mouse(packet.x, packet.y);
			swap_mouse_video();
		}

		break;
	case STATE_2_DEFAULT:
		if(packet.lb==0){
			move_mouse(packet.x, packet.y);
			swap_mouse_video();
		}
		else{
			int board_x;
			int board_y;
			coord_to_real_board(mouse->x+packet.x, mouse->y-packet.y, & board_x, & board_y);
			if(board[board_x][board_y]==2){
				int real_x, real_y;
				coord_to_real_board(mouse->x+packet.x, mouse->y-packet.y, & real_x, & real_y);
				mouse->hold=1;
				mouse->radius=BOARD_CL_RADIUS;
				mouse->color_circle=BOARD_PL2_CL_CLR;
				mouse->x_circle=mouse->x+packet.x-(BOARD_XI+real_x*BOARD_SQ_SIZE+BOARD_SQ_SIZE/2);
				mouse->y_circle=mouse->y-packet.y-(BOARD_YI+real_y*BOARD_SQ_SIZE+BOARD_SQ_SIZE/2);
				mouse->original_x=real_x;
				mouse->original_y=real_y;
				put_square(real_x, real_y, BOARD_ODD_SQ_CLR);
				state=STATE_2_HOLD_PIECE;
			}
				move_mouse(packet.x, packet.y);
				swap_mouse_video();
		}
		break;
	case STATE_2_HOLD_PIECE:
		if(packet.lb==0){
			mouse->hold=0;
			swap_double_mouse();
			int board_x, board_y;
			coord_to_real_board(mouse->x+packet.x, mouse->y-packet.y, & board_x, & board_y);
			int xi=mouse->original_x;
			int yi=mouse->original_y;
			int r=moveDisc(board, 2, xi, yi, board_x, board_y);
			if(r==0 || r==5 || r==6){
				put_circle(mouse->original_x, mouse->original_y, BOARD_PL2_CL_CLR);
				state=STATE_2_DEFAULT;
			}
			else if(r==1 || r==2){
				put_circle(board_x, board_y, BOARD_PL2_CL_CLR);
				state=STATE_1_DEFAULT;
				if(serial==1){
					if(send_serial_move(2, mouse->original_x, mouse->original_y, board_x, board_y)!=0){
						return 1;
					}
				}
				draw_turn_message(TURN_MSG_XI_YELLOW, TURN_MSG_YI,TURN_MSG_YELLOW_BACKGROUND_CLR, yellowturn);
			}
			else if(r==3){
				put_square(board_x-1, board_y-1, BOARD_ODD_SQ_CLR);
				put_circle(board_x, board_y,BOARD_PL2_CL_CLR);
				state=STATE_1_DEFAULT;
				if(serial==1){
					if(send_serial_move(2, mouse->original_x, mouse->original_y, board_x, board_y)!=0){
						return 1;
					}
				}
				draw_turn_message(TURN_MSG_XI_YELLOW, TURN_MSG_YI,TURN_MSG_YELLOW_BACKGROUND_CLR, yellowturn);
			}
			else if(r==4){
				put_square(board_x+1, board_y-1, BOARD_ODD_SQ_CLR);
				put_circle(board_x, board_y, BOARD_PL2_CL_CLR);
				state=STATE_1_DEFAULT;
				if(serial==1){
					if(send_serial_move(2, mouse->original_x, mouse->original_y, board_x, board_y)!=0){
						return 1;
					}

				}
				draw_turn_message(TURN_MSG_XI_YELLOW, TURN_MSG_YI,TURN_MSG_YELLOW_BACKGROUND_CLR, yellowturn);
			}
			move_mouse(packet.x,packet.y);
			swap_mouse_video();
			}
		else{
			move_mouse(packet.x, packet.y);
			swap_mouse_video();
		}
		break;
	}
	return 0;
}

//SERIAL FUNCTIONS
int send_serial_move(int player, int xi, int yi, int xf, int yf){
	if(player==1){
		if(ser_send_char_poll(PORT_ADDR, 'a',0)!=0){
			printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
			return 1;
		}
		if(ser_send_char_poll(PORT_ADDR, xi,0)!=0){
					printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
					return 1;
				}
		if(ser_send_char_poll(PORT_ADDR, yi,0)!=0){
					printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
					return 1;
				}
		if(ser_send_char_poll(PORT_ADDR, xf,0)!=0){
					printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
					return 1;
				}
		if(ser_send_char_poll(PORT_ADDR, yf,0)!=0){
					printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
					return 1;
				}
		printf("Serial, move message success pl: %d\n",1);
		return 0;
	}
	else if(player==2){
		if(ser_send_char_poll(PORT_ADDR, 'b',0)!=0){
			printf("Proj Error: Could not connect to player 1\n Make sure player 1 only runs the program BEFORE you.\n");
			return 1;
		}
		if(ser_send_char_poll(PORT_ADDR, xi,0)!=0){
					printf("Proj Error: Could not connect to player 1\n Make sure player 1 only runs the program BEFORE you.\n");
					return 1;
				}
		if(ser_send_char_poll(PORT_ADDR, yi,0)!=0){
							printf("Proj Error: Could not connect to player 1\n Make sure player 1 only runs the program BEFORE you.\n");
							return 1;
						}
		if(ser_send_char_poll(PORT_ADDR, xf,0)!=0){
							printf("Proj Error: Could not connect to player 1\n Make sure player 1 only runs the program BEFORE you.\n");
							return 1;
						}
		if(ser_send_char_poll(PORT_ADDR, yf,0)!=0){
							printf("Proj Error: Could not connect to player 1\n Make sure player 1 only runs the program BEFORE you.\n");
							return 1;
						}
		printf("Serial, move message success pl: %d\n",2);
		return 0;
	}
	else{
		printf("Serial, move message fail: player can only be 1 or 2\n");
		return 1;
	}
}

int send_serial_ret(int r){
	if(ser_send_char_poll(PORT_ADDR, r, 0)!=0){
		printf("Fail serial ret r: %d\n",r);
		return 1;
	}
	if(ser_send_char_poll(PORT_ADDR, 't', 0)!=0){
		printf("Fail serial ret t\n");
		return 1;
	}
	printf("Serial ret message success\n");
	return 0;
}

int wait_for_serial_message(int player,int * r,int * xi,int * yi,int * xf,int * yf){
	if(player==1){
	unsigned long c1, c2, c3, c4, c5, c6 , c7;
	if(ser_receive_char_poll(PORT_ADDR,& c1, 10000)!=0){
				printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
				return 1;
			}
	printf("wait message player 1 c1: %d\n",c1);
	if(ser_receive_char_poll(PORT_ADDR, & c2, 10000)!=0){
		printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
					return 1;
				}
	printf("wait message player 1 c2: %d\n",c2);
	if(c2=='t'){
		* r=c1;
		printf("Wait message success only two characters pl:%d r:%d xi:%d yi:%d xf:%d yf:%d\n",player, *r, *xi, *yi, *xf, *yf);
		return 0;
	}
	else if(c1!='b'){
		printf("Wait message fail c1 is not b pl:%d c1:%d c2:%d c3:%d c4:%d c5:%d c6:%d c7:%d\n",player, c1, c2, c3, c4, c5, c6, c7);
		return 1;
	}
	if(ser_receive_char_poll(PORT_ADDR,& c3, 10000)!=0){
					printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
					return 1;
				}

		if(ser_receive_char_poll(PORT_ADDR, & c4, 10000)!=0){
			printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
						return 1;
					}
		if(ser_receive_char_poll(PORT_ADDR,& c5, 10000)!=0){
							printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
							return 1;
						}

				if(ser_receive_char_poll(PORT_ADDR, & c6, 10000)!=0){
					printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
								return 1;
							}
				if(ser_receive_char_poll(PORT_ADDR,& c7, 10000)!=0){
									printf("Proj Error: Could not connect to player 2\n Make sure player 2 only runs the program AFTER you.\n");
									return 1;
								}
		if(c7!='t'){
			printf("Wait message fail c7 is not t pl:%d c1:%d c2:%d c3:%d c4:%d c5:%d c6:%d c7:%d\n",player, c1, c2, c3, c4, c5, c6, c7);
			return 1;
		}
		else{
			*xi=c2;
			*yi=c3;
			*xf=c4;
			*yf=c5;
			*r=c6;
			printf("Wait message success pl:%d r:%d xi:%d yi:%d xf:%d yf:%d\n",player, *r, *xi, *yi, *xf, *yf);
			return 0;
		}
	}
	else if(player==2){
		unsigned long c1, c2, c3, c4, c5, c6 , c7;
		if(ser_receive_char_poll(PORT_ADDR,& c1, 10000)!=0){
					printf("Proj Error: Could not connect to player 1\n Make sure player 1 only runs the program BEFORE you.\n");
					return 1;
				}
		printf("wait message player 2 c1: %d\n",c1);

		if(ser_receive_char_poll(PORT_ADDR, & c2, 10000)!=0){
			printf("Proj Error: Could not connect to player 1\n Make sure player 1 only runs the program BEFORE you.\n");
						return 1;
					}
		printf("wait message player 2 c2: %d\n",c2);
		if(c2=='t'){
			* r=c1;
			printf("Wait message success only two characters pl:%d r:%d xi:%d yi:%d xf:%d yf:%d\n",player, *r, *xi, *yi, *xf, *yf);
			return 0;
		}
		else if(c1!='a'){
			printf("Wait message fail c1 is not a pl:%d c1:%d c2:%d c3:%d c4:%d c5:%d c6:%d c7:%d\n",player, c1, c2, c3, c4, c5, c6, c7);
			return 1;
		}
		if(ser_receive_char_poll(PORT_ADDR,& c3, 10000)!=0){
			printf("Proj Error: Could not connect to player 1\n Make sure player 1 only runs the program BEFORE you.\n");
						return 1;
					}

			if(ser_receive_char_poll(PORT_ADDR, & c4, 10000)!=0){
				printf("Proj Error: Could not connect to player 1\n Make sure player 1 only runs the program BEFORE you.\n");
							return 1;
						}
			if(ser_receive_char_poll(PORT_ADDR,& c5, 10000)!=0){
				printf("Proj Error: Could not connect to player 1\n Make sure player 1 only runs the program BEFORE you.\n");
								return 1;
							}

					if(ser_receive_char_poll(PORT_ADDR, & c6, 10000)!=0){
						printf("Proj Error: Could not connect to player 1\n Make sure player 1 only runs the program BEFORE you.\n");
									return 1;
								}
					if(ser_receive_char_poll(PORT_ADDR,& c7, 10000)!=0){
						printf("Proj Error: Could not connect to player 1\n Make sure player 1 only runs the program BEFORE you.\n");
										return 1;
									}
			if(c7!='t'){
				printf("Wait message fail c7 is not t pl:%d c1:%d c2:%d c3:%d c4:%d c5:%d c6:%d c7:%d\n",player, c1, c2, c3, c4, c5, c6, c7);
				return 1;
			}
			else{
				*xi=c2;
				*yi=c3;
				*xf=c4;
				*yf=c5;
				*r=c6;
				printf("Wait message success pl:%d r:%d xi:%d yi:%d xf:%d yf:%d\n",player, *r, *xi, *yi, *xf, *yf);
				return 0;
			}
	}
	else{
		printf("Wait message fail: player can only be 1 or two\n");
		return 0;
	}
}

//DRAW FUNCTIONS

int draw_board(){
	int size=BOARD_SQ_SIZE;//size of each square of the board
	int xi=BOARD_XI;//initial x coordinate of the board
	int yi=BOARD_YI;//initial y coordinate of the board
	unsigned int i;
	for(i=0;i<8;i++){
		unsigned int j;
		for(j=0;j<8;j++){
			if(i%2==j%2){
				if(vg_draw_square(xi+i*size,yi+j*size,size,BOARD_EVEN_SQ_CLR)!=0){
					return 1;
				}
			}
			else{
				if(vg_draw_square(xi+i*size,yi+j*size,size,BOARD_ODD_SQ_CLR)!=0){
					return 1;
				}
				if(j<=2){
					if(vg_draw_circle(xi+i*size+size/2,yi+j*size+size/2,BOARD_CL_RADIUS,BOARD_PL2_CL_CLR)!=0){
						return 1;
					}
				}
				else if(j>=5){
					if(vg_draw_circle(xi+i*size+size/2,yi+j*size+size/2,BOARD_CL_RADIUS,BOARD_PL1_CL_CLR)!=0){
						return 1;
					}
				}
			}
		}
	}
	return 0;
}



int put_square(int board_x, int board_y, int color){
	int i;
	for(i=BOARD_XI+board_x*BOARD_SQ_SIZE;i<BOARD_XI+(board_x+1)*BOARD_SQ_SIZE;i++){
		int j;
		for(j=BOARD_YI+board_y*BOARD_SQ_SIZE;j<BOARD_YI+(board_y+1)*BOARD_SQ_SIZE;j++){
			vg_set_pixel(i,j,color);
		}
	}
}

int put_circle(int board_x, int board_y, int color){
	vg_draw_circle(BOARD_XI+board_x*BOARD_SQ_SIZE+BOARD_SQ_SIZE/2,BOARD_YI+board_y*BOARD_SQ_SIZE+BOARD_SQ_SIZE/2,BOARD_CL_RADIUS, color);
	return 0;
}

int move_piece(int player, int xi, int yi, int xf, int yf){
	int r=moveDisc(board, player, xi, yi, xf, yf);
	if(r==1 || r==2){
		put_square(xi, yi, BOARD_ODD_SQ_CLR);
		if(player==1){
			put_circle(xf, yf, BOARD_PL1_CL_CLR);
			return 0;
		}
		else if(player==2){
			put_circle(xf, yf, BOARD_PL2_CL_CLR);
			return 0;
		}
	return 1;
	}
	else if(r==3){
		put_square(xi, yi, BOARD_ODD_SQ_CLR);
		if(player==1){
			put_square(xf-1, yf+1, BOARD_ODD_SQ_CLR);
			put_circle(xf, yf, BOARD_PL1_CL_CLR);
			return 0;
		}
		else if(player==2){
			put_square(xf-1, yf-1, BOARD_ODD_SQ_CLR);
			put_circle(xf, yf, BOARD_PL2_CL_CLR);
			return 0;
		}
		else{
			return 1;
		}
	}
	else if(r==4){
		put_square(xi, yi, BOARD_ODD_SQ_CLR);
		if(player==1){
			put_square(xf+1, yf+1, BOARD_ODD_SQ_CLR);
			put_circle(xf, yf, BOARD_PL1_CL_CLR);
			return 0;
		}
		else if(player==2){
			put_square(xf+1, yf-1, BOARD_ODD_SQ_CLR);
			put_circle(xf, yf, BOARD_PL2_CL_CLR);
			return 0;
		}
		else{
			return 1;
		}
	}
	else{
		return 1;
	}
}





int draw_turn_message(int x, int y, unsigned int background_clr, Bitmap * turn){
	int ii,jj;//counter
	for(ii=BOARD_XI;ii<BOARD_XI+8*BOARD_SQ_SIZE;ii++){
		for(jj=0;jj<BOARD_YI;jj++){
			unsigned char * bitmap_buffer=background->bitmapData+((background->bitmapInfoHeader.height-1-jj)*background->bitmapInfoHeader.width+ii)*2;
			unsigned int color=((*bitmap_buffer)| *(bitmap_buffer+1)<<8);
			vg_set_pixel(ii,jj,color);
		}
	}
	unsigned char * bitmap_buffer=turn->bitmapData;
	int i;//counter
	for(i=y+turn->bitmapInfoHeader.height-1;i>=y;i--){
		int j;//counter
		for(j=x;j<x+turn->bitmapInfoHeader.width;j++){
			unsigned int color=((*bitmap_buffer) | *(bitmap_buffer+1)<<8);
			//printf("color : %x",color);
			if(color!=background_clr){
				vg_set_pixel(j,i,color);
			}
			bitmap_buffer=bitmap_buffer+2;
		}
	}
}

//GENERIC FUNCTIONS
int coord_to_real_board(int coord_x, int coord_y, int * board_x, int * board_y){
	*board_x=(coord_x-BOARD_XI)/BOARD_SQ_SIZE;
	*board_y=(coord_y-BOARD_YI)/BOARD_SQ_SIZE;
	return 0;
}

int state_to_player(int state){
	switch(state){
	case STATE_1_HOLD_PIECE:
		return 1;
		break;
	case STATE_1_DEFAULT:
		return 1;
		break;
	case STATE_2_HOLD_PIECE:
		return 2;
		break;
	case STATE_2_DEFAULT:
		return 2;
		break;
	}
}


//MOUSE FUNCTIONS

int create_mouse(int x, int y){
	if(mouse==NULL){
		mouse=(mouse_t *)malloc(sizeof(mouse_t));
		mouse->x=x;
		mouse->y=y;
		mouse->bitmap=NULL;
		mouse->hold=0;
		mouse->radius=0;
		mouse->color_circle=0;
		mouse->x_circle=0;
		mouse->y_circle=0;
		return 0;
	}
	else{
		return 1;
	}
}


int move_mouse(int x_delta, int y_delta){
	int i;
	for(i=mouse->x;i<=mouse->x+mouse->bitmap->bitmapInfoHeader.width;i++){
		int j;
		for(j=mouse->y;j<=mouse->y+mouse->bitmap->bitmapInfoHeader.height;j++){
			mouse_to_double(i,j);
		}
	}

	if(mouse->hold==1){
		int xi=mouse->x-mouse->x_circle;
		int yi=mouse->y-mouse->y_circle;
		for(i=xi-mouse->radius;i<xi+mouse->radius;i++){
			int j;
			for(j=yi-mouse->radius;j<yi+mouse->radius;j++){
				if((i-xi)*(i-xi)+(j-yi)*(j-yi)<=mouse->radius*mouse->radius){
					mouse_to_double(i,j);
				}
			}
		}
	}
	if(mouse->x+x_delta<0){
		mouse->x=0;
	}
	else if(mouse->x+x_delta>getH_res()-15){
		mouse->x=getH_res()-15;
	}
	else{
		mouse->x=mouse->x+x_delta;
	}
	if(mouse->y-y_delta<0){
		mouse->y=0;
	}
	else if(mouse->y-y_delta>getV_res()-20){
		mouse->y=getV_res()-20;
	}
	else{
		mouse->y=mouse->y-y_delta;
	}
	if(mouse->hold==1){
			draw_mouse_circle(mouse->x-mouse->x_circle, mouse->y-mouse->y_circle,mouse->radius, mouse->color_circle);
	}

	draw_mouse();
}

int draw_mouse(){
	unsigned char * bitmap_buffer=mouse->bitmap->bitmapData;
	int i;
	for(i=mouse->y+mouse->bitmap->bitmapInfoHeader.height-1;i>=mouse->y;i--){
		int j;
		for(j=mouse->x;j<mouse->x+mouse->bitmap->bitmapInfoHeader.width;j++){
			unsigned int color=((*bitmap_buffer) | *(bitmap_buffer+1)<<8);
			if(color!=MOUSE_TRANSPARENT_CLR_2){
				vg_set_mouse_pixel(j,i,color);
			}
			bitmap_buffer=bitmap_buffer+2;
		}
	}
	return 0;
}

int draw_mouse_circle(int xi, int yi,int radius, int color){
	int i;
	for(i=xi-radius;i<xi+radius;i++){
		int j;
		for(j=yi-radius;j<yi+radius;j++){
			if((i-xi)*(i-xi)+(j-yi)*(j-yi)<=radius*radius){
				if(vg_set_mouse_pixel(i,j,color)!=0){
					return 1;
				}
			}
		}
	}
	 return 0;
}



int delete_mouse(){
	free(mouse);
}
