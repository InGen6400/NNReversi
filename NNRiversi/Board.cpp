
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

	board->Sp = board->Stack;
	Stack_PUSH(board, STACK_STOP);
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
char Board_Flip(Board *board, char color, char pos) {

	char flipCount=0;

	if (board->Stone[pos] != NONE)return 0;

#pragma region dirSelectswitch

	switch (pos) {
	case C1:
	case C2:
	case D1:
	case D2:
	case E1:
	case E2:
	case F1:
	case F2:
		flipCount += Board_FlipLine(board, color, pos, DIR_L);
		flipCount += Board_FlipLine(board, color, pos, DIR_R);
		flipCount += Board_FlipLine(board, color, pos, DIR_DL);
		flipCount += Board_FlipLine(board, color, pos, DIR_D);
		flipCount += Board_FlipLine(board, color, pos, DIR_DR);
		break;
	case C8:
	case C7:
	case D8:
	case D7:
	case E8:
	case E7:
	case F8:
	case F7:
		flipCount += Board_FlipLine(board, color, pos, DIR_UL);
		flipCount += Board_FlipLine(board, color, pos, DIR_U);
		flipCount += Board_FlipLine(board, color, pos, DIR_UR);
		flipCount += Board_FlipLine(board, color, pos, DIR_L);
		flipCount += Board_FlipLine(board, color, pos, DIR_R);
		break;
	case A3:
	case A4:
	case A5:
	case A6:
	case B3:
	case B4:
	case B5:
	case B6:
		flipCount += Board_FlipLine(board, color, pos, DIR_U);
		flipCount += Board_FlipLine(board, color, pos, DIR_UR);
		flipCount += Board_FlipLine(board, color, pos, DIR_R);
		flipCount += Board_FlipLine(board, color, pos, DIR_D);
		flipCount += Board_FlipLine(board, color, pos, DIR_DR);
		break;
	case H3:
	case H4:
	case H5:
	case H6:
	case G3:
	case G4:
	case G5:
	case G6:
		flipCount += Board_FlipLine(board, color, pos, DIR_UL);
		flipCount += Board_FlipLine(board, color, pos, DIR_U);
		flipCount += Board_FlipLine(board, color, pos, DIR_L);
		flipCount += Board_FlipLine(board, color, pos, DIR_DL);
		flipCount += Board_FlipLine(board, color, pos, DIR_D);
		break;
	case A1:
	case A2:
	case B1:
	case B2:
		flipCount += Board_FlipLine(board, color, pos, DIR_R);
		flipCount += Board_FlipLine(board, color, pos, DIR_D);
		flipCount += Board_FlipLine(board, color, pos, DIR_DR);
		break;
	case A8:
	case A7:
	case B8:
	case B7:
		flipCount += Board_FlipLine(board, color, pos, DIR_U);
		flipCount += Board_FlipLine(board, color, pos, DIR_UR);
		flipCount += Board_FlipLine(board, color, pos, DIR_R);
		break;
	case H1:
	case H2:
	case G1:
	case G2:
		flipCount += Board_FlipLine(board, color, pos, DIR_L);
		flipCount += Board_FlipLine(board, color, pos, DIR_DL);
		flipCount += Board_FlipLine(board, color, pos, DIR_D);
		break;
	case H8:
	case H7:
	case G8:
	case G7:
		flipCount += Board_FlipLine(board, color, pos, DIR_UL);
		flipCount += Board_FlipLine(board, color, pos, DIR_U);
		flipCount += Board_FlipLine(board, color, pos, DIR_L);
		break;
	default:
		flipCount += Board_FlipLine(board, color, pos, DIR_UL);
		flipCount += Board_FlipLine(board, color, pos, DIR_U);
		flipCount += Board_FlipLine(board, color, pos, DIR_UR);
		flipCount += Board_FlipLine(board, color, pos, DIR_L);
		flipCount += Board_FlipLine(board, color, pos, DIR_R);
		flipCount += Board_FlipLine(board, color, pos, DIR_DL);
		flipCount += Board_FlipLine(board, color, pos, DIR_D);
		flipCount += Board_FlipLine(board, color, pos, DIR_DR);
		break;
	}

#pragma endregion

	if (flipCount>0) {
		board->Stone[pos] = color;
		Stack_PUSH(board, pos);
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
char Board_FlipLine(Board *board, char color, char pos, char vec) {
	char oppColor = getOppStone(color);
	char flipCount = 0;

	pos += vec;

	if (board->Stone[pos] != oppColor)return 0;

	pos += vec;

	if (board->Stone[pos] == oppColor) {
		pos += vec;
		if (board->Stone[pos] == oppColor) {
			pos += vec;
			if (board->Stone[pos] == oppColor) {
				pos += vec;
				if (board->Stone[pos] == oppColor) {
					pos += vec;
					if (board->Stone[pos] == oppColor) {
						pos += vec;
						if (board->Stone[pos] != color) {
							return 0;
						}
						pos -= vec;
						flipCount++;
						board->Stone[pos] = color;
						Stack_PUSH(board, pos);
					}
					else if (board->Stone[pos] != color) {
						return 0;
					}
					pos -= vec;
					flipCount++;
					board->Stone[pos] = color;
					Stack_PUSH(board, pos);
				}
				else if (board->Stone[pos] != color) {
					return 0;
				}
				pos -= vec;
				flipCount++;
				board->Stone[pos] = color;
				Stack_PUSH(board, pos);
			}
			else if (board->Stone[pos] != color) {
				return 0;
			}
			pos -= vec;
			flipCount++;
			board->Stone[pos] = color;
			Stack_PUSH(board, pos);
		}
		else if (board->Stone[pos] != color) {
			return 0;
		}
		pos -= vec;
		flipCount++;
		board->Stone[pos] = color;
		Stack_PUSH(board, pos);
	}
	else if (board->Stone[pos] != color) {
		return 0;
	}
	pos -= vec;
	flipCount++;
	board->Stone[pos] = color;
	Stack_PUSH(board, pos);

	return flipCount;
}

char Board_CanPlay(Board *board, char color) {
	int x, y;
	for (y = 1; y <= BOARD_SIZE; y++) {
		for (x = 1; x <= BOARD_SIZE; x++) {
			if (Board_CanFlip(board, color, x, y)) {
				return TRUE;
			}
		}
	}
	return FALSE;
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

char Board_CountFlips(const Board *board, char in_color, char in_pos) {
	return 0;
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
	return BLACK + WHITE - color;
}

char Stack_POP(Board *board) {
	return *(--board->Sp);
}

void Stack_PUSH(Board *board, char num) {
	*(board->Sp++) = num;
}