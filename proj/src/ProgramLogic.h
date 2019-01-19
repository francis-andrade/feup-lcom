#ifndef __PROGRAMLOGIC_H
#define __PROGRAMLOGIC_H

/** @defgroup ProgramLogic ProgramLogic
 * @{
 * Functions for the logical part of the game
 */

/**
 * @brief Creates the board with initial positions.
 *
 *
 * 0 - empty position;
 *
 * 1 - black discs (player 1);
 *
 * 2 - white discs (player 2).
 *
 * @param board[8][8] array of arrays with current game status.
 * @return returns 0 if successful.
 */
int createBoard(char board[8][8]);

/**
 * @brief Moves disc in the given position(xi,yi) to final position also given(xf,yf).
 * @param board[8][8] array of arrays with current game status;
 * @param player the player who is going to play (1-player with black discs; 2-player with white discs);
 * @param xi initial position x component;
 * @param yi initial position y component;
 * @param xf final position x component;
 * @param yf final position y component.
 * @return 0 if invalid play;
 * @return 1 if valid play - move disc to right side;
 * @return 2 if valid play - move disc to left side;
 * @return 3 if valid play - capture disc to right side;
 * @return 4 if valid play - capture disc to left side;
 * @return 5 if invalid play - has to capture a disc;
 * @return 6 if invalid play - has to capture a disc with another initial position.
 */
int moveDisc(char board[8][8], char player, int xi, int yi, int xf, int yf);

/**
 * @brief Verifies if given initial position(xi,yi) is valid or not.
 * @param board[8][8] array of arrays with current game status;
 * @param player the player who is going to play (1-player with black discs; 2-player with white discs);
 * @param xi initial position x component;
 * @param yi initial position y component.
 * @return 0 if valid position;
 * @return 1 if invalid position.
 */
int isPositionInitialValid(char board[8][8], char player, int xi, int yi);

/**
 * @brief Verifies if given initial position(xi,yi) can do a capture move or not.
 * @param board[8][8] array of arrays with current game status;
 * @param player the player who is going to play (1-player with black discs; 2-player with white discs);
 * @param xi initial position x component;
 * @param yi initial position y component.
 * @return 0 if cant make any cature moves;
 * @return 1 if can do a capture move to right side;
 * @return 2 if can do a capture move to left side;
 * @return 3 if can do a capture move to both sides.
 */
int verifyCaptureMove(char board[8][8], char player, int xi, int yi);

/**
 * @brief Moves disc in the given position(xi,yi) to final position also given(xf,yf).
 * @param board[8][8] array of arrays with current game status;
 * @param xi initial position x component;
 * @param yi initial position y component;
 * @param xf final position x component;
 * @param yf final position y component.
 */
void moveToFinal(char board[8][8], int xi, int yi, int xf, int yf);

/**
 * @brief Moves disc in the given position(xi,yi) to final position also given(xf,yf), making a capture move.
 * @param board[8][8] -> array of arrays with current game status;
 * @param xi initial position x component;
 * @param yi initial position y component;
 * @param xf final position x component;
 * @param yf final position y component.
 */
void makeCaptureMove(char board[8][8], int xi, int yi, int xf, int yf);

/**
 * @brief Verifies if it is the end of the game.
 * @param board[8][8] -> array of arrays with current game status.
 * @return 0 if it's not the end of the game;
 * @return 1 if it's the end of the game.
 */
int isEndGame(char board[8][8]);


/**
 * @brief Gives the moves that a disc can make.
 * @param board[8][8] array of arrays with current game status;
 * @param player the player who is going to play (1-player with black discs; 2-player with white discs);
 * @return returns an array of integers(r[5]) with the positions we can move to:
 * @return r[0] number of positions we can use(0,1 or 2);
 * @return r[1] and r[2] x and y, respectively, of a position we can move to (used if r[0]==1 or r[0]==2);
 * @return r[3] and r[4] x and y, respectively, of a position we can move to (used if r[0]==2).
 *
 */
int * movesICanDo(char board[8][8], char player, int xi, int yi);
/** @} end of ProgramLogic */
#endif
