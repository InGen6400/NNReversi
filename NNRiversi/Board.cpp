
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Board.h"
#include "const.h"

//ボード生成
Board *Board_New(void) {
	Board *res;
	res = (Board*)malloc(sizeof(Board));
	if (res) {
		Board_Reset(res);
	}
	return res;
}

//ボードの削除
void Board_Delete(Board *board) {
	free(board);
}

//盤面の初期化
void Board_Reset(Board *board) {
	int y, x;
	for (y = 0; y < BOARD_SIZE + 2; y++) {
		for (x = 0; x < BOARD_SIZE + 2; x++) {
			if (y == 0 || x == 0 || y == BOARD_SIZE + 1 || x == BOARD_SIZE + 1) {
				board->Piece[ConvertPos(x,y)] = WALL;
			}
		}
	}

	for (y = 1; y <= BOARD_SIZE; y++) {
		for (x = 1; x <= BOARD_SIZE; x++) {
			board->Piece[ConvertPos(y, x)] = NONE;
		}
	}

	board->Piece[D4] = BLACK;
	board->Piece[E5] = BLACK;
	board->Piece[E4] = WHITE;
	board->Piece[D5] = WHITE;
}

//盤面の描画
void Board_Draw(Board *board) {
	int x, y;
	char icon[5];
	printf("\n\n");
	printf(" 　　　　Ａ  Ｂ  Ｃ  Ｄ  Ｅ  Ｆ  Ｇ  Ｈ\n");
	printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
	for (y = 0; y < BOARD_SIZE + 2; y++) {
		if (y != 0 && y != 9) {
			printf(" %d ｜", y);
		}
		else{
			printf("   ｜");
		}
		for (x = 0; x < BOARD_SIZE + 2; x++) {
			switch (board->Piece[ConvertPos(x, y)]) {
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

//駒を数える
int	Board_CountPieces(const Board *board, char color) {
	int x, y, sum = 0;
	for (y = 1; y <= BOARD_SIZE; y++) {
		for (x = 1; x <= BOARD_SIZE; x++) {
			if (board->Piece[ConvertPos(x, y)] == color) {
				sum++;
			}
		}
	}
	return sum;
}

//着手
int	Board_Flip(Board *board, char color, int x, int y) {

	int flipCount=0;

	if (x <= 0 || y <= 0 || x > BOARD_SIZE || y > BOARD_SIZE)return 0;
	if (board->Piece[ConvertPos(x, y)] != NONE)return 0;

	flipCount += Board_FlipLine(board, color, x, y, 1, 0);
	flipCount += Board_FlipLine(board, color, x, y, 1, 1);
	flipCount += Board_FlipLine(board, color, x, y, 0, 1);
	flipCount += Board_FlipLine(board, color, x, y, -1, 1);
	flipCount += Board_FlipLine(board, color, x, y, -1, 0);
	flipCount += Board_FlipLine(board, color, x, y, -1, -1);
	flipCount += Board_FlipLine(board, color, x, y, 0, -1);
	flipCount += Board_FlipLine(board, color, x, y, 1, -1);
	if (flipCount>0) {
		board->Piece[ConvertPos(x, y)] = color;
	}

	return flipCount;
}

//直線を裏返す
int Board_FlipLine(Board *board, char color, int x, int y, int vec_x, int vec_y) {
	char oppColor = getOppStone(color);
	int flips[10];
	int flipCount = 0;
	int i;

	x += vec_x;
	y += vec_y;

	if (x <= 0 || y <= 0 || x > BOARD_SIZE || y > BOARD_SIZE)return 0;
	if (board->Piece[ConvertPos(x, y)] != oppColor)return 0;
	flips[flipCount] = ConvertPos(x, y);
	flipCount++;

	x += vec_x;
	y += vec_y;

	while (x >= 1 && y >= 1 && x <= BOARD_SIZE && y <= BOARD_SIZE) {
		if (board->Piece[ConvertPos(x, y)] == NONE)return 0;
		if (board->Piece[ConvertPos(x, y)] == color) {
			//裏返す
			for (i = 0; i < flipCount; i++) {
				board->Piece[flips[i]] = color;
			}
			return flipCount;
		}
		flips[flipCount] = ConvertPos(x, y);
		flipCount++;
		x += vec_x;
		y += vec_y;
	}
	return 0;
}

//一手戻す
int	Board_Reflip(Board *board) {
	return 0;
}

//posに着手できるか
int	Board_CanFlip(const Board *board, char color, int x, int y) {

	if (x <= 0 && y <= 0 && x > BOARD_SIZE && y > BOARD_SIZE)return FALSE;
	if (board->Piece[ConvertPos(x, y)] != NONE)return FALSE;

	if(Board_CanFlipLine(board, color, x, y, 1, 0))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, 1, 1))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, 0, 1))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, -1, 1))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, -1, 0))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, -1, -1))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, 0, -1))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, 1, -1))return TRUE;

	return FALSE;
	return 0;
}

//Board_CanFlipの補助
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

//x,y から インデックス座標へ
int ConvertPos(int x, int y) {
	return x + 10 * y;
}

//x座標を返す
int getX(int pos) {
	return pos % 10;
}

//y座標を返す
int getY(int pos) {
	return pos / 10;
}

//反転色を返す　1->2 2->1 (0->1)
int getOppStone(char color) {
	return color % 2 + 1;
}