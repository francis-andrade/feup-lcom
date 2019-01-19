#include "ProgramLogic.h"
#include <stdio.h>

int createBoard(char board[8][8])
{
	/*
	 * 0 - vazio
	 * 1 - preto
	 * 2 - branco
	 */

	//Coluna 0
	board[0][0] = 0;
	board[0][1] = 2;
	board[0][2] = 0;
	board[0][3] = 0;
	board[0][4] = 0;
	board[0][5] = 1;
	board[0][6] = 0;
	board[0][7] = 1;

	//Coluna 1
	board[1][0] = 2;
	board[1][1] = 0;
	board[1][2] = 2;
	board[1][3] = 0;
	board[1][4] = 0;
	board[1][5] = 0;
	board[1][6] = 1;
	board[1][7] = 0;

	//Coluna 2
	board[2][0] = 0;
	board[2][1] = 2;
	board[2][2] = 0;
	board[2][3] = 0;
	board[2][4] = 0;
	board[2][5] = 1;
	board[2][6] = 0;
	board[2][7] = 1;

	//Coluna 3
	board[3][0] = 2;
	board[3][1] = 0;
	board[3][2] = 2;
	board[3][3] = 0;
	board[3][4] = 0;
	board[3][5] = 0;
	board[3][6] = 1;
	board[3][7] = 0;

	//Coluna 4
	board[4][0] = 0;
	board[4][1] = 2;
	board[4][2] = 0;
	board[4][3] = 0;
	board[4][4] = 0;
	board[4][5] = 1;
	board[4][6] = 0;
	board[4][7] = 1;

	//Coluna 5
	board[5][0] = 2;
	board[5][1] = 0;
	board[5][2] = 2;
	board[5][3] = 0;
	board[5][4] = 0;
	board[5][5] = 0;
	board[5][6] = 1;
	board[5][7] = 0;

	//Coluna 6
	board[6][0] = 0;
	board[6][1] = 2;
	board[6][2] = 0;
	board[6][3] = 0;
	board[6][4] = 0;
	board[6][5] = 1;
	board[6][6] = 0;
	board[6][7] = 1;

	//Coluna 7
	board[7][0] = 2;
	board[7][1] = 0;
	board[7][2] = 2;
	board[7][3] = 0;
	board[7][4] = 0;
	board[7][5] = 0;
	board[7][6] = 1;
	board[7][7] = 0;

	return 0;
}

int isPositionInitialValid(char board[8][8], char player, int xi, int yi){
	if (board[xi][yi] != player)
		return 1; //o jogador não pode usar essa peça
	else if (board[xi+1][yi-1] == 1 && board[xi-1][yi-1] == 1
			&& player == 1)
		return 1;
	else if (board[xi+1][yi + 1] == 2 && board[xi - 1][yi + 1] == 2
			&& player == 2)
		return 1;
	else {
		int i;
		int j;
		int y;
		y = 0; // o jogador pode usar essa peça;
		for (i = 0; i < 8; i++)
			for (j = 0; j < 8; j++) {
				if (board[j][i] == player) {
					if (verifyCaptureMove(board, player, j, i) != 0) {
						if (j == xi && i == yi) {
							return 0; //o jogador pode usar essa peça
						}
						y = 2; // o jogador tem uma peça para comer que não corresponde à peça inicial
					}
				}
			}
		return y;
	}
}



int moveDisc(char board[8][8], char player, int xi, int yi, int xf, int yf) {
	/*char player:
	 * 1 - jogador com peças pretas
	 * 2- jogador com peças brancas
	 */
	int i;
	int j;
	int y;
	y = 0; // o jogador pode usar essa peça;
	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++) {
			if (board[j][i] == player) {
				if (verifyCaptureMove(board, player, j, i) != 0) {
					if (j == xi && i == yi) {
						y = 1; //o jogador pode usar essa peça
					}
					if (y != 1)
						y = 2; // o jogador tem uma peça para comer que não corresponde à peça inicial
				}
			}
		}
	if (y==2)
		return 6;

	if (player == 1 && board[xi][yi] == 1) {
		if (xf == xi + 1 && yf == yi - 1
				&& verifyCaptureMove(board, 1, xi, yi) == 0
				&& board[xf][yf] == 0) {
			moveToFinal(board, xi, yi, xf, yf);
			return 1; // jogada válida - move a peça para o lado direito
		} else if (xf == xi - 1 && yf == yi - 1
				&& verifyCaptureMove(board, 1, xi, yi) == 0
				&& board[xf][yf] == 0) {
			moveToFinal(board, xi, yi, xf, yf);
			return 2; // jogada válida - move a peça para o lado esquerdo

		} else if ((xf == xi + 1 || xf == xi - 1) && yf == yi - 1
				&& verifyCaptureMove(board, 1, xi, yi) != 0
				&& board[xf][yf] == 0) {
			return 5; // jogada inválida - obrigado a comer
		} else if (xf == xi + 2 && yf == yi - 2
				&& (verifyCaptureMove(board, 1, xi, yi) == 1
						|| verifyCaptureMove(board, 1, xi, yi) == 3)) {
			makeCaptureMove(board, xi, yi, xf, yf);
			return 3; // jogada válida - come a peça para o lado direito
		} else if (xf == xi - 2 && yf == yi - 2
				&& (verifyCaptureMove(board, 1, xi, yi) == 2
						|| verifyCaptureMove(board, 1, xi, yi) == 3)) {
			makeCaptureMove(board, xi, yi, xf, yf);
			return 4; // jogada válida - come a peça para o lado esquerdo
		} else
			return 0; // jogada inválida
	}
	if (player == 2) {
		if (xf == xi + 1 && yf == yi + 1
				&& verifyCaptureMove(board, 2, xi, yi) == 0
				&& board[xf][yf] == 0) {
			moveToFinal(board, xi, yi, xf, yf);
			return 1; // jogada válida - move a peça para o lado direito
		} else if (xf == xi - 1 && yf == yi + 1
				&& verifyCaptureMove(board, 2, xi, yi) == 0
				&& board[xf][yf] == 0) {
			moveToFinal(board, xi, yi, xf, yf);
			return 2; // jogada válida - move a peça para o lado esquerdo
		} else if ((xf == xi + 1 || xf == xi - 1) && yf == yi + 1
				&& verifyCaptureMove(board, 2, xi, yi) != 0
				&& board[xf][yf] == 0) {
			return 5; // jogada inválida - obrigado a comer
		} else if (xf == xi + 2 && yf == yi + 2
				&& (verifyCaptureMove(board, 2, xi, yi) == 1
						|| verifyCaptureMove(board, 2, xi, yi) == 3)) {
			makeCaptureMove(board, xi, yi, xf, yf);
			return 3; // jogada válida - come a peça para o lado direito
		} else if (xf == xi - 2 && yf == yi + 2
				&& (verifyCaptureMove(board, 2, xi, yi) == 2
						|| verifyCaptureMove(board, 2, xi, yi) == 3)) {
			makeCaptureMove(board, xi, yi, xf, yf);
			return 4; // jogada válida - come a peça para o lado esquerdo
		} else
			return 0; // jogada inválida
	}
	return 0;
}

int isEndGame(char board[8][8]){
	int i;
	for (i=0; i< 8; i++)
	{
		if (board[i][0] == 1)
			return 1; // jogador 1 ganhou
		if (board[i][7] == 2)
			return 2; //jogador 2 ganhou
	}
	return 0; // não termina o jogo

}

void moveToFinal(char board[8][8], int xi, int yi, int xf, int yf){
	int x;
	x = board[xi][yi];
	board[xi][yi]=0;
	board[xf][yf]=x;
}

void makeCaptureMove(char board[8][8], int xi, int yi, int xf, int yf){
	int x;
	x=board[xi][yi];
	board[xi][yi] = 0;
	if (xf-xi>0){
		if (yf-yi>0){
			board[xi+1][yi+1]=0;
			board[xf][yf]=x;
		}
		else{
			board[xi+1][yi-1]=0;
			board[xf][yf]=x;
		}
	}
	else {
		if (yf - yi > 0) {
			board[xi - 1][yi + 1] = 0;
			board[xf][yf] = x;
		} else {
			board[xi - 1][yi - 1] = 0;
			board[xf][yf] = x;
		}
	}
}

int verifyCaptureMove(char board[8][8], char player, int xi, int yi) {
	int x;
	x = 0; // não tem nada para comer
	if (player == 1) {
		if (xi + 1 < 8 && yi - 1 >= 0 && xi + 2 < 8 && yi - 2 >= 0)
			if (board[xi + 1][yi - 1] == 2 && board[xi + 2][yi - 2] == 0)
				x = 1; //pode comer para a direita
		if (xi - 1 >= 0 && yi - 1 >= 0 && xi - 2 >= 0 && yi - 2 >= 0)
			if (board[xi - 1][yi - 1] == 2 && board[xi - 2][yi - 2] == 0)
				x += 2; //se x=2, pode comer para a esquerda, se 3 pode comer para ambos os lados
	}
	if (player == 2) {
		if (xi + 1 < 8 && yi + 1 < 8 && xi + 2 < 8 && yi + 2 < 8)
			if (board[xi + 1][yi + 1] == 1 && board[xi + 2][yi + 2] == 0)
				x = 1; //pode comer para a direita
		if (xi - 1 >= 0 && yi + 1 < 8 && xi - 2 >= 0 && yi + 2 < 8)
			if (board[xi - 1][yi + 1] == 1 && board[xi - 2][yi + 2] == 0)
				x += 2; //se x=2, pode comer para a esquerda, se 3 pode comer para ambos os lados
	}
	return x;
}

int * movesICanDo(char board[8][8], char player, int xi, int yi){
	static int r[5];
	if (isPositionInitialValid(board, player, xi, yi)){
		r[0]=0;
		return r;
	}

	if (player==0){
		r[0]=0;
		return r;
	}
	int x;
	x = verifyCaptureMove(board, player, xi,yi);
	if (x == 1){
		r[0]=1;
		r[1]=xi+2;
		if (player==1)
			r[2]=yi-2;
		else
			r[2]=yi+2;
		return r;
	}
	if (x==2){
		r[0] = 1;
		r[1] = xi - 2;
		if (player == 1)
			r[2] = yi - 2;
		else
			r[2] = yi + 2;
		return r;
	}
	if (x==3){
		r[0]=2;
		r[1] = xi + 2;
		r[3] = xi - 2;
		if (player == 1){
			r[2] = yi - 2;
			r[4] = yi - 2;
		}
		else{
			r[2] = yi + 2;
			r[4] = yi + 2;
		}
		return r;
	}
	if (x==0){
		if (player==1){
			int nPos;
			nPos = 0;
			if (xi + 1 < 8 && yi - 1 >= 0 && board[xi + 1][yi - 1] == 0) {
				nPos += 1;
				r[1] = xi+1;
				r[2] = yi-1;
			}
			if (xi - 1 >= 0 && yi - 1 >= 0 && board[xi - 1][yi - 1] == 0) {
				nPos += 1;
				if (nPos == 2){
					r[3]=xi-1;
					r[4]=yi-1;
				}
				else{
					r[1]=xi-1;
					r[2]=yi-1;
				}
			}
			r[0]=nPos;
			return r;
		}
		if (player == 2) {
			int nPos;
			nPos = 0;
			if (xi + 1 < 8 && yi + 1 < 8 && board[xi + 1][yi + 1] == 0) {
				nPos += 1;
				r[1] = xi + 1;
				r[2] = yi + 1;
			}
			if (xi - 1 >= 0 && yi + 1 < 8 && board[xi - 1][yi + 1] == 0) {
				nPos += 1;
				if (nPos == 2) {
					r[3] = xi - 1;
					r[4] = yi + 1;
				} else {
					r[1] = xi - 1;
					r[2] = yi + 1;
				}
			}
			r[0] = nPos;
			return r;
		}
	}
	return r;
}
