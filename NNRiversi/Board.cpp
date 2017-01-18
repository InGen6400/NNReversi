
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Board.h"

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
	printf(" 　＋ー＋ー＋ー＋ー＋ー＋ー＋ー＋ー＋ー＋ー＋\n");
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
		printf(" 　＋ー＋ー＋ー＋ー＋ー＋ー＋ー＋ー＋ー＋ー＋\n");
	}
}

int	Board_getPiece(const Board *board, int in_pos);

int	Board_CountPieces(const Board *board, int in_color);

int	Board_Flip(Board *board, int in_color, int in_pos);

int	Board_Reflip(Board *board);

int	Board_CanFlip(const Board *board, int in_color, int in_pos);

int	Board_CountFlips(const Board *board, int in_color, int in_pos);

int Board_getPos(int x, int y) {
	return x + 10 * y;
}