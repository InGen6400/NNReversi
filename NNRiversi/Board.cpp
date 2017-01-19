
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Board.h"
#include "const.h"

Board *Board_New(void) {
	Board *res;
	res = (Board*)malloc(sizeof(Board));
	if (res) {
		Board_Reset(res);
	}
	return res;
}

void Board_Delete(Board *board) {
	free(board);
}

void Board_Reset(Board *board) {
	int i, j;
	for (i = 0; i < BOARD_SIZE + 2; i++) {
		for (j = 0; j < BOARD_SIZE + 2; j++) {
			if (i == 0 || j == 0 || i == BOARD_SIZE + 1 || j == BOARD_SIZE + 1) {
				board->Piece[Board_getPos(i,j)] = WALL;
			}
		}
	}

	for (i = 1; i <= BOARD_SIZE; i++) {
		for (j = 1; j <= BOARD_SIZE; j++) {
			board->Piece[Board_getPos(i, j)] = NONE;
		}
	}

	board->Piece[D4] = BLACK;
	board->Piece[E5] = BLACK;
	board->Piece[E4] = WHITE;
	board->Piece[D5] = WHITE;
}

void Board_Draw(Board *board) {
	int i, j;
	char icon[5];
	printf(" 　　　　Ａ  Ｂ  Ｃ  Ｄ  Ｅ  Ｆ  Ｇ  Ｈ\n");
	printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
	for (i = 0; i < BOARD_SIZE + 2; i++) {
		if (i != 0 && i != 9) {
			printf(" %d ｜", i);
		}
		else{
			printf("   ｜");
		}
		for (j = 0; j < BOARD_SIZE + 2; j++) {
			switch (board->Piece[Board_getPos(i, j)]) {
			case WALL:
				strcpy(icon, "＃");
				break;
			case NONE:
				strcpy(icon, "　");
				break;
			case WHITE:
				strcpy(icon, "●");
				break;
			case BLACK:
				strcpy(icon, "〇");
				break;
			}
			printf("%s｜", icon);
		}
		putchar('\n');
		printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
	}
}

int	Board_getPiece(const Board *board, int pos) {

	return board->Piece[pos];

}

int	Board_CountPieces(const Board *board, char color) {
	int i, j, sum = 0;
	for (i = 1; i <= BOARD_SIZE; i++) {
		for (j = 1; j <= BOARD_SIZE; j++) {
			if (board->Piece[Board_getPos(i, j)] == color) {
				sum++;
			}
		}
	}
	return sum;
}

int	Board_Flip(Board *board, char color, int x, int y) {

	int flipCount=0;

	if (x <= 0 && y <= 0 && x > BOARD_SIZE && y > BOARD_SIZE)return 0;
	if (board->Piece[ConvertPos(x, y)] != NONE)return 0;

	flipCount += Board_FlipLine(board, color, x, y, 1, 0);
	flipCount += Board_FlipLine(board, color, x, y, 1, 1);
	flipCount += Board_FlipLine(board, color, x, y, 0, 1);
	flipCount += Board_FlipLine(board, color, x, y, -1, 1);
	flipCount += Board_FlipLine(board, color, x, y, -1, 0);
	flipCount += Board_FlipLine(board, color, x, y, -1, -1);
	flipCount += Board_FlipLine(board, color, x, y, 0, -1);
	flipCount += Board_FlipLine(board, color, x, y, 1, -1);

	return flipCount;
}

int Board_FlipLine(Board *board, char color, int x, int y, int vec_x, int vec_y) {
	char oppColor = getOppStone(color);
	int flips[10];
	int flipCount = 0;

	x += vec_x;
	y += vec_y;

	if (x <= 0 && y <= 0 && x > BOARD_SIZE && y > BOARD_SIZE)return 0;
	if (board->Piece[ConvertPos(x, y)] == NONE)return 0;
	if (board->Piece[ConvertPos(x, y)] == color)return 0;

	x += vec_x;
	y += vec_y;

	while (x >= 1 && y >= 1 && x <= BOARD_SIZE && y <= BOARD_SIZE) {
		if (board->Piece[ConvertPos(x, y)] == NONE)return 0;
		if (board->Piece[ConvertPos(x, y)] == color)return TRUE;
		x += vec_x;
		y += vec_y;
	}
	return 0;
}

int	Board_Reflip(Board *board) {
	return 0;
}

int	Board_CanFlip(const Board *board, char color, int x, int y) {

	if (x <= 0 && y <= 0 && x > BOARD_SIZE && y > BOARD_SIZE)return FALSE;
	if (board->Piece[ConvertPos(x, y)] != NONE)return FALSE;

	flipCount += Board_CanFlipLine(board, color, x, y, 1, 0))return TRUE;
	flipCount += Board_CanFlipLine(board, color, x, y, 1, 1))return TRUE;
	flipCount += Board_CanFlipLine(board, color, x, y, 0, 1))return TRUE;
	flipCount += Board_CanFlipLine(board, color, x, y, -1, 1))return TRUE;
	flipCount += Board_CanFlipLine(board, color, x, y, -1, 0))return TRUE;
	flipCount += Board_CanFlipLine(board, color, x, y, -1, -1))return TRUE;
	flipCount += Board_CanFlipLine(board, color, x, y, 0, -1))return TRUE;
	flipCount += Board_CanFlipLine(board, color, x, y, 1, -1))return TRUE;

	return FALSE;
	return 0;
}

int Board_CanFlipLine(const Board *board, char color, int x, int y, int vec_x, int vec_y) {
	char oppColor = getOppStone(color);

	x += vec_x;
	y += vec_y;

	if (x <= 0 && y <= 0 && x > BOARD_SIZE && y > BOARD_SIZE)return FALSE;
	if (board->Piece[ConvertPos(x, y)] == NONE)return FALSE;
	if (board->Piece[ConvertPos(x, y)] == color)return FALSE;

	x += vec_x;
	y += vec_y;

	while (x >= 1 && y >= 1 && x <= BOARD_SIZE && y <= BOARD_SIZE) {
		if (board->Piece[ConvertPos(x, y)] == NONE)return FALSE;
		if (board->Piece[ConvertPos(x, y)] == color)return TRUE;
		x += vec_x;
		y += vec_y;
	}
	return FALSE;
}

int	Board_CountFlips(const Board *board, char color, int pos) {
	return 0;
}

int ConvertPos(int x, int y) {
	return x + 10 * y;
}

int getX(int pos) {
	return pos % 10;
}

int getY(int pos) {
	return pos / 10;
}

//1->2 2->1 (0->1)
int getOppStone(char color) {
	return color % 2 + 1;
}