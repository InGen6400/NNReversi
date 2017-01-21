﻿
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Board.h"
#include "const.h"
#include "Container.h"

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
	char y, x;
	for (y = 0; y < BOARD_SIZE + 2; y++) {
		for (x = 0; x < BOARD_SIZE + 2; x++) {
			if (y == 0 || x == 0 || y == BOARD_SIZE + 1 || x == BOARD_SIZE + 1) {
				board->Stone[ConvertPos(x,y)] = WALL;
			}
		}
	}

	for (y = 1; y <= BOARD_SIZE; y++) {
		for (x = 1; x <= BOARD_SIZE; x++) {
			board->Stone[ConvertPos(y, x)] = NONE;
		}
	}

	board->Stone[D4] = BLACK;
	board->Stone[E5] = BLACK;
	board->Stone[E4] = WHITE;
	board->Stone[D5] = WHITE;

	board->White = 2;
	board->Black = 2;

	Board_EmptyListInit(board);

	board->Sp = board->Stack;
	Stack_PUSH(board, STACK_STOP);
}

void Board_EmptyListInit(Board *board) {
	char poslist[] = {
		A1, A8, H8, H1,
		D3, D6, E3, E6, C4, C5, F4, F5,
		C3, C6, F3, F6,
		D2, D7, E2, E7, B4, B5, G4, G5,
		C2, C7, F2, F7, B3, B6, G3, G6,
		D1, D8, E1, E8, A4, A5, H4, H5,
		C1, C8, F1, F8, A3, A6, H3, H6,
		B2, B7, G2, G7,
		B1, B8, G1, G8, A2, A7, H2, H7,
		D4, D5, E4, E5
	};
	int i = 0;
	charNode *node = board->isEmpty;
	node->value = NOMOVE;
	node->prev = NULL;
	node->next = NULL;
	for (i = 0; i <= BOARD_SIZE*BOARD_SIZE; i++) {
		if (board->Stone[poslist[i]] == NONE) {
			node[1].value = poslist[i];
			node[1].prev = node;
			node[1].next = NULL;
			node->next = &node[1];
			node++;
		}
	}
}

//盤面の描画
void Board_Draw(Board *board) {
	char x, y;
	char icon[5];
	printf("\n\n");
	printf(" 　　　　Ａ  Ｂ  Ｃ  Ｄ  Ｅ  Ｆ  Ｇ  Ｈ                        黒:%d  vs  白:%d\n", board->Black, board->White);
	printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
	for (y = 0; y < BOARD_SIZE + 2; y++) {
		if (y != 0 && y != 9) {
			printf(" %d ｜", y);
		}
		else{
			printf("   ｜");
		}
		for (x = 0; x < BOARD_SIZE + 2; x++) {
			switch (board->Stone[ConvertPos(x, y)]) {
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

//着手
char Board_Flip(Board *board, char color, char x, char y) {

	char flipCount=0;

	if (x <= 0 || y <= 0 || x > BOARD_SIZE || y > BOARD_SIZE)return 0;
	if (board->Stone[ConvertPos(x, y)] != NONE)return 0;

	flipCount += Board_FlipLine(board, color, x, y, 1, 0);
	flipCount += Board_FlipLine(board, color, x, y, 1, 1);
	flipCount += Board_FlipLine(board, color, x, y, 0, 1);
	flipCount += Board_FlipLine(board, color, x, y, -1, 1);
	flipCount += Board_FlipLine(board, color, x, y, -1, 0);
	flipCount += Board_FlipLine(board, color, x, y, -1, -1);
	flipCount += Board_FlipLine(board, color, x, y, 0, -1);
	flipCount += Board_FlipLine(board, color, x, y, 1, -1);

	if (flipCount>0) {
		board->Stone[ConvertPos(x, y)] = color;
		Stack_PUSH(board, ConvertPos(x, y));
		Stack_PUSH(board, flipCount);
		Stack_PUSH(board, color);

		if (color == WHITE) {
			board->White += flipCount + 1;
			board->Black -= flipCount;
		}
		else {
			board->White -= flipCount;
			board->Black += flipCount + 1;
		}
	}

	return flipCount;
}

//直線を裏返す
char Board_FlipLine(Board *board, char color, char x, char y, char vec_x, char vec_y) {
	char oppColor = getOppStone(color);
	char flips[10];
	char flipCount = 0;
	char i;

	x += vec_x;
	y += vec_y;

	if (x <= 0 || y <= 0 || x > BOARD_SIZE || y > BOARD_SIZE)return 0;
	if (board->Stone[ConvertPos(x, y)] != oppColor)return 0;
	flips[flipCount] = ConvertPos(x, y);
	flipCount++;

	x += vec_x;
	y += vec_y;

	while (x >= 1 && y >= 1 && x <= BOARD_SIZE && y <= BOARD_SIZE) {
		if (board->Stone[ConvertPos(x, y)] == NONE)return 0;
		if (board->Stone[ConvertPos(x, y)] == color) {
			//実際に裏返す
			for (i = 0; i < flipCount; i++) {
				board->Stone[flips[i]] = color;
				Stack_PUSH(board, flips[i]);
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

//posに着手できるか
char Board_CanFlip(const Board *board, char color, char x, char y) {

	if (x <= 0 && y <= 0 && x > BOARD_SIZE && y > BOARD_SIZE)return FALSE;
	if (board->Stone[ConvertPos(x, y)] != NONE)return FALSE;

	if(Board_CanFlipLine(board, color, x, y, 1, 0))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, 1, 1))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, 0, 1))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, -1, 1))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, -1, 0))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, -1, -1))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, 0, -1))return TRUE;
	if(Board_CanFlipLine(board, color, x, y, 1, -1))return TRUE;

	return FALSE;
}

//Board_CanFlipの補助
char Board_CanFlipLine(const Board *board, char color, char x, char y, char vec_x, char vec_y) {
	char oppColor = getOppStone(color);

	x += vec_x;
	y += vec_y;

	if (x <= 0 && y <= 0 && x > BOARD_SIZE && y > BOARD_SIZE)return FALSE;
	if (board->Stone[ConvertPos(x, y)] == NONE)return FALSE;
	if (board->Stone[ConvertPos(x, y)] == color)return FALSE;

	x += vec_x;
	y += vec_y;

	while (x >= 1 && y >= 1 && x <= BOARD_SIZE && y <= BOARD_SIZE) {
		if (board->Stone[ConvertPos(x, y)] == NONE)return FALSE;
		if (board->Stone[ConvertPos(x, y)] == color)return TRUE;
		x += vec_x;
		y += vec_y;
	}
	return FALSE;
}

//一手戻す
void Board_Undo(Board *board) {
	char color = Stack_POP(board);
	char opp = getOppStone(color);
	char flipCount = Stack_POP(board);
	char pos;

	board->Stone[Stack_POP(board)] = NONE;
	
	if (color == WHITE) {
		board->White -= flipCount + 1;
		board->Black += flipCount;
	}
	else {
		board->White += flipCount;
		board->Black -= flipCount + 1;
	}

	for (char i = 0; i < flipCount; i++) {
		board->Stone[Stack_POP(board)] = opp;
	}
}

char Board_CountStone(const Board *board, char in_color) {
	if (in_color == WHITE) {
		return board->White;
	}
	else {
		return board->Black;
	}
	return -1;
}

//x,y から インデックス座標へ
char ConvertPos(char x, char y) {
	return x + 10 * y;
}

//x座標を返す
char getX(char pos) {
	return pos % 10;
}

//y座標を返す
char getY(char pos) {
	return pos / 10;
}

//反転色を返す　1->2 2->1 (0->1)
char getOppStone(char color) {
	return color % 2 + 1;
}

char Stack_POP(Board *board) {
	return *(--board->Sp);
}

void Stack_PUSH(Board *board, char num) {
	*(board->Sp++) = num;
}