#ifndef __CHECKERS_H
#define __CHECKERS_H

/** @defgroup Checkers Checkers
 * @{
 * Functions for construction and manipulation of the board
 */

#include "Bitmap.h"
#include "mouse.h"



/*#define BOARD_XI 300
#define BOARD_YI 100
#define BOARD_SQ_SIZE 64
#define BOARD_CL_RADIUS 20
#define BOARD_EVEN_SQ_CLR 0
#define BOARD_ODD_SQ_CLR 63
#define BOARD_PL1_CL_CLR 4
#define BOARD_PL2_CL_CLR 6*/

#define CHECKERS_TIMEOUT 30
#define WHITE_CLR 0xFFFF
#define BLACK_CLR 0
#define BOARD_XI 320
#define BOARD_YI 192
#define BOARD_SQ_SIZE 80
#define BOARD_CL_RADIUS 32
#define BOARD_EVEN_SQ_CLR 0xC618
#define BOARD_ODD_SQ_CLR 0x18C9
#define BOARD_PL1_CL_CLR 0xEEC4 /**< @brief E8D820 */
#define BOARD_PL2_CL_CLR 0xC945 /**< @brief C82828 */
#define MOUSE_TRANSPARENT_CLR 0xF81F
#define MOUSE_TRANSPARENT_CLR_2 0x0001
#define TURN_MSG_CLR 0x1383
#define TURN_MSG_YELLOW_BACKGROUND_CLR 0xE6A5
#define TURN_MSG_RED_BACKGROUND_CLR 0xC146
#define TURN_MSG_XI_RED 540
#define TURN_MSG_XI_YELLOW 480
#define TURN_MSG_YI 100

/**
 * This struct represents the mouse that will be used to play checkers
 */
typedef struct{
	int x;
	int y;
	Bitmap * bitmap;
	int hold;
	int radius;
	int color_circle;
	int x_circle;
	int y_circle;
	int original_x;
	int original_y;

}__attribute__((packed)) mouse_t;

/**
 * States of the game
 */
enum
{
STATE_1_HOLD_PIECE,/**< @brief when player 1 is holding a piece and it's his turn */
STATE_1_DEFAULT,/**< @brief when it's player 1 turn and he is not doing anything */
STATE_2_HOLD_PIECE,/**< @brief when player 2 is holding a piece and it's his turn */
STATE_2_DEFAULT/**< @brief when it's player 2 turn and he is not doing anything */

};



/**
 * @brief loads the images and starts the game
 * @param path_images path of y«the directory that the contains the images
 * @param player_1 set to 1 if player_1 is currently playing, o otherwise
 * @param player_2 set to 1 if player_2 is currently playing
 * @return 1 in case of success, 0 otherwise
 */
int checkers_start_game(char * path_images, int player_1, int player_2);
/**
 * @brief handles the game in serial mode being responsible for handling which player should play
 * @param player_1 set to 1 if player_1 is currently playing, o otherwise
 * @param player_2 set to 1 if player_2 is currently playing
 * @return 1 in case of success, 0 otherwise
 */
int checkers_serial_handler(int player_1, int player_2);
/**
 * @brief handles the interrupts
 * @param player_1 set to 1 if player_1 is currently playing, o otherwise
 * @param player_2 set to 1 if player_2 is currently playing
 * @return 1 in case of success, 0 otherwise
 */
int checkers_int_handler(int player1, int player2);
/**
 * @brief handles the moves
 * @param player_1 set to 1 if player_1 is currently playing, o otherwise
 * @param player_2 set to 1 if player_2 is currently playing
 * @return 1 in case of success, 0 otherwise
 */
int checkers_move_handler(mouse_packet_t packet, int player1, int player2);

//Serial
/**
 * @brief Sends a message saying that a player did a move
 * @param player player that made the move
 * @param xi initial x position
 * @param yi initial y position
 * @param xf final x position
 * @param yf final y position
 * @return 1 in case of success, 0 otherwise
 */
int send_serial_move(int player, int xi, int yi, int xf, int yf);
/**
 * @brief sends a serial message saying that a player finished making a move with the corresponding return value
 * @param r return value
 * @return 1 in case of success, 0 otherwise
 */
int send_serial_ret(int r);
/**
 * @brief Waits for a message from the serial port
 * @param player player that is waiting for the message
 * @param r return value
 * @param xi initial x position
 * @param yi initial y position
 * @param xf final x position
 * @param yf final y position
 * @return 1 in case of success, 0 otherwise
 */
int wait_for_serial_message(int player,int * r,int * xi,int * yi,int * xf,int * yf);

//DRAW FUNCTIONS
/**
 * @brief draws a board on the screen
 * @return 1 in case of success, 0 otherwise
 */
int draw_board();
/**
 * @brief places an empty square
 * @param board_x x coordinate
 * @param board_y y coordinate
 * @param color color of the square
 * @return 0 in case of success, 1 otherwise
 */
int put_square(int board_x, int board_y, int color);
/**
 * @brief places a circle
 * @param board_x x coordinate
 * @param board_y y coordinate
 * @param color color of the square
 * @return 0 in case of success, 1 otherwise
 */
int put_circle(int board_x, int board_y, int color);
/**
 * @brief moves a piece and handles everything pertinent related to the move
 * @param player player that made the move
 * @param xi initial x position
 * @param yi initial y position
 * @param xf final x position
 * @param yf final y position
 * @return 1 in case of success, 0 otherwise
 */
int move_piece(int player, int xi, int yi, int xf, int yf);
/**
 * @brief draws the turn message that says who should play now
 */
int draw_turn_message(int x, int y, unsigned int background_clr, Bitmap * turn);



//GENERIC FUNCTIONS
/**
 * @brief converts screen coordinates to board coordinates
 * @param coord_x screen coordinate x
 * @param coord_y screen coordinate y
 * @param board_x board coordinate x
 * @param board_y board coordinate y
 * @return 0 in case of success, 1 otherwise
 */
int coord_to_real_board(int coord_x, int coord_y, int * board_x, int * board_y);
/**
 * @brief converts a state to the corresponding player
 */
int state_to_player(int state);

//MOUSE FUNCTIONS
/*
 * @brief creates a mouse initially in (x,y)
 */
int create_mouse(int x, int y);
/**
 * @brief moves mouse (x_delta, y_delta)
 */
int move_mouse(int x_delta, int y_delta);
/**
 * @brief draws a mouse
 */
int draw_mouse();
/**
 * @brief draws the the mouse circle
 */
int draw_mouse_circle(int xi, int yi,int radius, int color);

/** @}* end of checkers */

#endif
