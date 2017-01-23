
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BitBoard.h"
#include "const.h"
#include "Container.h"
#include "CPU.h"

//ボード生成
BitBoard *BitBoard_New(void) {
	BitBoard *res;
	res = (BitBoard*)malloc(sizeof(BitBoard));
	if (res) {
		BitBoard_Reset(res);
	}
	return res;
}

//ボードの削除
void BitBoard_Delete(BitBoard *bitboard) {
	free(bitboard);
}

//盤面の初期化
void BitBoard_Reset(BitBoard *bitboard) {
	bitboard->white = 0x8042201008040201;//真ん中二つ以外0
	bitboard->black = 0x0000000810000000;//真ん中二つ以外0
}

//盤面の描画
void BitBoard_Draw(const BitBoard *bitboard) {
	char x, y;

	BitBoard cpyBoard = *bitboard;

	printf("\n\n");
	printf(" 　　　　Ａ  Ｂ  Ｃ  Ｄ  Ｅ  Ｆ  Ｇ  Ｈ                        黒:  vs  白:\n"/*, board->Black, board->White*/);
	printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
	printf(" 　｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜\n");
	printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");



	for (x = 0; x < BitBOARD_SIZE * BitBOARD_SIZE; x++, cpyBoard.white<<=1, cpyBoard.black<<=1) {
		if (x%8 == 0) {
			printf(" %d ｜＃｜", x/8+1);
		}
		if (cpyBoard.white & 0x8000000000000000) {
			printf("●｜");
		}
		else if (cpyBoard.black & 0x8000000000000000) {
			printf("〇｜");
		}
		else {
			printf("　｜");
		}
		if (x % 8 == 7) {

			printf("＃｜ %d", x / 8+1);
			putchar('\n');
			printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
		}
	}
	printf(" 　｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜\n");
	printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
	printf(" 　　　　Ａ  Ｂ  Ｃ  Ｄ  Ｅ  Ｆ  Ｇ  Ｈ\n");

}

//colorの石数を返す
char BitBoard_CountStone(const BitBoard *bitboard, char in_color) {
	return 0;
}

//着手
char BitBoard_Flip(BitBoard *bitboard, char color, char pos) {

	char flipCount = 0;

	return flipCount;
}

//直線を裏返す
char BitBoard_FlipLine(BitBoard *bitboard, char color, char pos, char vec) {
	char oppColor = getOppStone(color);
	char flipCount = 0;

	pos += vec;

	return flipCount;
}

//一手戻す
void BitBoard_Undo(BitBoard *bitboard) {
	char pos;
}

//着手できるか
char BitBoard_CanPlay(BitBoard *bitboard, char color) {
	int i;

	return FALSE;
}

//posに着手できるか
char BitBoard_CanFlip(const BitBoard *bitboard, char color, char pos) {



	return FALSE;
}

//Board_CanFlipの補助
char BitBoard_CanFlipLine(const BitBoard *bitboard, char color, char pos, char vec) {
	char oppColor = getOppStone(color);
	char count = 0;

	return count;
}

//反転数を返す
char BitBoard_CountFlips(const BitBoard *bitboard, char color, char pos) {
	char flipCount = 0;


	return flipCount;
}

uint64 getBitPos(char x, char y) {
	return (uint64)1 << (x + y * BOARD_SIZE);
}



