
//2017.02.16  1405 +- 5

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BitBoard.h"
#include "const.h"
#include "Container.h"
#include "CPU.h"
#include <intrin.h>

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
	bitboard->stone[WHITE] = 0x0000001008000000;//真ん中二つ以外0
	bitboard->stone[BLACK] = 0x0000000810000000;//真ん中二つ以外0
	//bitboard->stone[WHITE] = 0xc000800000000001;//
	//bitboard->stone[BLACK] = 0x00c0400000000000;//
	//角のインデックスが22011021になるはず
	bitboard->Sp = bitboard->Stack;
}

//盤面の描画
void BitBoard_Draw(const BitBoard *bitboard, char isMob) {
	char x, y;

	BitBoard cpyBoard = *bitboard;

	uint64 mobility_w = BitBoard_getMobility(cpyBoard.stone[WHITE], cpyBoard.stone[BLACK]);
	uint64 mobility_b = BitBoard_getMobility(cpyBoard.stone[BLACK], cpyBoard.stone[WHITE]);

	printf("\n\n");
	printf(" 　　　　Ａ  Ｂ  Ｃ  Ｄ  Ｅ  Ｆ  Ｇ  Ｈ                        黒:%d  vs  白:%d\n", BitBoard_CountStone(bitboard->stone[BLACK]), BitBoard_CountStone(bitboard->stone[WHITE]));
	printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
	printf(" 　｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜\n");
	printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");

	for (x = BITBOARD_SIZE * BITBOARD_SIZE - 1; x >= 0; x--) {
		if ((x+1)%8 == 0) {
			printf(" %d ｜＃｜", 8-x/8);
		}
		if ((cpyBoard.stone[WHITE]>>x) & 1) {
			printf("●｜");
		}
		else if ((cpyBoard.stone[BLACK]>>x) & 1) {
			printf("〇｜");
		}
		else if (((mobility_w>>x) & 1) && isMob) {
			printf("．｜");
		}
		else if (((mobility_b>>x) & 1) && isMob) {
			printf("。｜");
		}
		else {
			printf("　｜");
		}
		if ((x+1) % 8 == 1) {

			printf("＃｜ %d", x / 8+1);
			putchar('\n');
			printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
		}
	}
	printf(" 　｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜\n");
	printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
	printf(" 　　　　Ａ  Ｂ  Ｃ  Ｄ  Ｅ  Ｆ  Ｇ  Ｈ\n");

}

//colorの石数を返す(立っているビット数を返す)
char BitBoard_CountStone(uint64 bits) {
	/*bits = bits - (bits>>1 & 0x5555555555555555);
	bits = (bits & 0x3333333333333333) + (bits >> 2 & 0x3333333333333333);
	bits = (bits & 0x0F0F0F0F0F0F0F0F) + (bits >> 4 & 0x0F0F0F0F0F0F0F0F);
	bits = (bits & 0x00FF00FF00FF00FF) + (bits >> 8 & 0x00FF00FF00FF00FF);
	bits = (bits & 0x0000FFFF0000FFFF) + (bits >> 16 & 0x0000FFFF0000FFFF);
	return (bits & 0x00000000FFFFFFFF) + (bits >> 32 & 0x00000000FFFFFFFF);*/
	return _mm_popcnt_u64(bits);
}

//posに着手する
char BitBoard_Flip(BitBoard *bitboard, char color, uint64 pos) {
	uint64 reverse;
	uint64 *me = &bitboard->stone[color];
	uint64 *ene = &bitboard->stone[(color+1)&1];

	reverse = getReverseBits(me, ene, pos);
	//drawBits(pos);

	//反転可能なら着手
	if (reverse != 0) {
		*me ^= pos | reverse;
		*ene ^= reverse;
		Stack_PUSH(bitboard, pos);
		Stack_PUSH(bitboard, reverse);
		Stack_PUSH(bitboard, color);
		return 1;
	}
	else {
		printf("iligal position\n");
	}

	return 0;
}

//反転するビットを返す(要高速化)
inline uint64 getReverseBits(const uint64 *me, const uint64 *ene, const uint64 pos) {
	
	if (((*ene | *me) & pos) != 0)return 0;
	uint64 revBits = 0;
	const uint64 wh = *ene & 0x7E7E7E7E7E7E7E7E;
	const uint64 wv = *ene & 0x00FFFFFFFFFFFF00;

	//右探索6マス   
	revBits |= (pos >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3) | (*me << 4) | (*me << 5) | (*me << 6));
	revBits |= (pos >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3) | (*me << 4) | (*me << 5));
	revBits |= (pos >> 3) & (wh >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3) | (*me << 4));
	revBits |= (pos >> 4) & (wh >> 3) & (wh >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3));
	revBits |= (pos >> 5) & (wh >> 4) & (wh >> 3) & (wh >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2));
	revBits |= (pos >> 6) & (wh >> 5) & (wh >> 4) & (wh >> 3) & (wh >> 2) & (wh >> 1) & wh & (*me << 1);

	//下探索6マス
	revBits |= (pos >> 8) & wv & ((*me << 8) | (*me << 16) | (*me << 24) | (*me << 32) | (*me << 40) | (*me << 48));
	revBits |= (pos >> 16) & (wv >> 8) & wv & ((*me << 8) | (*me << 16) | (*me << 24) | (*me << 32) | (*me << 40));
	revBits |= (pos >> 24) & (wv >> 16) & (wv >> 8) & wv & ((*me << 8) | (*me << 16) | (*me << 24) | (*me << 32));
	revBits |= (pos >> 32) & (wv >> 24) & (wv >> 16) & (wv >> 8) & wv & ((*me << 8) | (*me << 16) | (*me << 24));
	revBits |= (pos >> 40) & (wv >> 32) & (wv >> 24) & (wv >> 16) & (wv >> 8) & wv & ((*me << 8) | (*me << 16));
	revBits |= (pos >> 48) & (wv >> 40) & (wv >> 32) & (wv >> 24) & (wv >> 16) & (wv >> 8) & wv & (*me << 8);

	//右下探索6マス
	revBits |= (pos >> 9) & wh & ((*me << 9) | (*me << 18) | (*me << 27) | (*me << 36) | (*me << 45) | (*me << 54));
	revBits |= (pos >> 18) & (wh >> 9) & wh & ((*me << 9) | (*me << 18) | (*me << 27) | (*me << 36) | (*me << 45));
	revBits |= (pos >> 27) & (wh >> 18) & (wh >> 9) & wh & ((*me << 9) | (*me << 18) | (*me << 27) | (*me << 36));
	revBits |= (pos >> 36) & (wh >> 27) & (wh >> 18) & (wh >> 9) & wh & ((*me << 9) | (*me << 18) | (*me << 27));
	revBits |= (pos >> 45) & (wh >> 36) & (wh >> 27) & (wh >> 18) & (wh >> 9) & wh & ((*me << 9) | (*me << 18));
	revBits |= (pos >> 54) & (wh >> 45) & (wh >> 36) & (wh >> 27) & (wh >> 18) & (wh >> 9) & wh & (*me << 9);

	//左下探索6マス
	revBits |= (pos >> 7) & wh & ((*me << 7) | (*me << 14) | (*me << 21) | (*me << 28) | (*me << 35) | (*me << 42));
	revBits |= (pos >> 14) & (wh >> 7) & wh & ((*me << 7) | (*me << 14) | (*me << 21) | (*me << 28) | (*me << 35));
	revBits |= (pos >> 21) & (wh >> 14) & (wh >> 7) & wh & ((*me << 7) | (*me << 14) | (*me << 21) | (*me << 28));
	revBits |= (pos >> 28) & (wh >> 21) & (wh >> 14) & (wh >> 7) & wh & ((*me << 7) | (*me << 14) | (*me << 21));
	revBits |= (pos >> 35) & (wh >> 28) & (wh >> 21) & (wh >> 14) & (wh >> 7) & wh & ((*me << 7) | (*me << 14));
	revBits |= (pos >> 42) & (wh >> 35) & (wh >> 28) & (wh >> 21) & (wh >> 14) & (wh >> 7) & wh & (*me << 7);

	//左探索6マス
	revBits |= (pos << 1) & wh & ((*me >> 1) | (*me >> 2) | (*me >> 3) | (*me >> 4) | (*me >> 5) | (*me >> 6));
	revBits |= (pos << 2) & (wh << 1) & wh & ((*me >> 1) | (*me >> 2) | (*me >> 3) | (*me >> 4) | (*me >> 5));
	revBits |= (pos << 3) & (wh << 2) & (wh << 1) & wh & ((*me >> 1) | (*me >> 2) | (*me >> 3) | (*me >> 4));
	revBits |= (pos << 4) & (wh << 3) & (wh << 2) & (wh << 1) & wh & ((*me >> 1) | (*me >> 2) | (*me >> 3));
	revBits |= (pos << 5) & (wh << 4) & (wh << 3) & (wh << 2) & (wh << 1) & wh & ((*me >> 1) | (*me >> 2));
	revBits |= (pos << 6) & (wh << 5) & (wh << 4) & (wh << 3) & (wh << 2) & (wh << 1) & wh & (*me >> 1);

	//上探索6マス
	revBits |= (pos << 8) & wv & ((*me >> 8) | (*me >> 16) | (*me >> 24) | (*me >> 32) | (*me >> 40) | (*me >> 48));
	revBits |= (pos << 16) & (wv << 8) & wv & ((*me >> 8) | (*me >> 16) | (*me >> 24) | (*me >> 32) | (*me >> 40));
	revBits |= (pos << 24) & (wv << 16) & (wv << 8) & wv & ((*me >> 8) | (*me >> 16) | (*me >> 24) | (*me >> 32));
	revBits |= (pos << 32) & (wv << 24) & (wv << 16) & (wv << 8) & wv & ((*me >> 8) | (*me >> 16) | (*me >> 24));
	revBits |= (pos << 40) & (wv << 32) & (wv << 24) & (wv << 16) & (wv << 8) & wv & ((*me >> 8) | (*me >> 16));
	revBits |= (pos << 48) & (wv << 40) & (wv << 32) & (wv << 24) & (wv << 16) & (wv << 8) & wv & (*me >> 8);

	//右上探索6マス
	revBits |= (pos << 7) & wh & ((*me >> 7) | (*me >> 14) | (*me >> 21) | (*me >> 28) | (*me >> 35) | (*me >> 42));
	revBits |= (pos << 14) & (wh << 7) & wh & ((*me >> 7) | (*me >> 14) | (*me >> 21) | (*me >> 28) | (*me >> 35));
	revBits |= (pos << 21) & (wh << 14) & (wh << 7) & wh & ((*me >> 7) | (*me >> 14) | (*me >> 21) | (*me >> 28));
	revBits |= (pos << 28) & (wh << 21) & (wh << 14) & (wh << 7) & wh & ((*me >> 7) | (*me >> 14) | (*me >> 21));
	revBits |= (pos << 35) & (wh << 28) & (wh << 21) & (wh << 14) & (wh << 7) & wh & ((*me >> 7) | (*me >> 14));
	revBits |= (pos << 42) & (wh << 35) & (wh << 28) & (wh << 21) & (wh << 14) & (wh << 7) & wh & (*me >> 7);

	//左上探索6マス
	revBits |= (pos << 9) & wh & ((*me >> 9) | (*me >> 18) | (*me >> 27) | (*me >> 36) | (*me >> 45) | (*me >> 54));
	revBits |= (pos << 18) & (wh << 9) & wh & ((*me >> 9) | (*me >> 18) | (*me >> 27) | (*me >> 36) | (*me >> 45));
	revBits |= (pos << 27) & (wh << 18) & (wh << 9) & wh & ((*me >> 9) | (*me >> 18) | (*me >> 27) | (*me >> 36));
	revBits |= (pos << 36) & (wh << 27) & (wh << 18) & (wh << 9) & wh & ((*me >> 9) | (*me >> 18) | (*me >> 27));
	revBits |= (pos << 45) & (wh << 36) & (wh << 27) & (wh << 18) & (wh << 9) & wh & ((*me >> 9) | (*me >> 18));
	revBits |= (pos << 54) & (wh << 45) & (wh << 36) & (wh << 27) & (wh << 18) & (wh << 9) & wh & (*me >> 9);

	return revBits;
}

//一手戻す(未実装)
int BitBoard_Undo(BitBoard *bitboard) {
	if (bitboard->Sp <= bitboard->Stack)return 0;
	char color = Stack_POP(bitboard);
	uint64 rev = Stack_POP(bitboard);
	bitboard->stone[color] ^= rev | Stack_POP(bitboard);
	bitboard->stone[oppColor(color)] ^= rev;
	return 1;
}

//着手可能位置(要高速化)
uint64 BitBoard_getMobility(uint64 me, uint64 ene) {

	uint64 blank = ~(me | ene);

	uint64 mobility;

	uint64 mask = ene & 0x7e7e7e7e7e7e7e7e;

	//左
	uint64 t = mask & (me << 1);

	t |= mask & (t << 1);
	t |= mask & (t << 1);
	t |= mask & (t << 1);
	t |= mask & (t << 1);
	t |= mask & (t << 1);

	mobility = blank & (t << 1);

	//右
	t = mask & (me >> 1);

	t |= mask & (t >> 1);
	t |= mask & (t >> 1);
	t |= mask & (t >> 1);
	t |= mask & (t >> 1);
	t |= mask & (t >> 1);

	mobility |= blank & (t >> 1);

	//下
	t = mask & (me >> 8);

	t |= mask & (t >> 8);
	t |= mask & (t >> 8);
	t |= mask & (t >> 8);
	t |= mask & (t >> 8);
	t |= mask & (t >> 8);

	mobility |= blank & (t >> 8);

	//上
	t = mask & (me << 8);

	t |= mask & (t << 8);
	t |= mask & (t << 8);
	t |= mask & (t << 8);
	t |= mask & (t << 8);
	t |= mask & (t << 8);

	mobility |= blank & (t << 8);

	//右下
	t = mask & (me >> 9);

	t |= mask & (t >> 9);
	t |= mask & (t >> 9);
	t |= mask & (t >> 9);
	t |= mask & (t >> 9);
	t |= mask & (t >> 9);

	mobility |= blank & (t >> 9);

	//左下
	t = mask & (me >> 7);

	t |= mask & (t >> 7);
	t |= mask & (t >> 7);
	t |= mask & (t >> 7);
	t |= mask & (t >> 7);
	t |= mask & (t >> 7);

	mobility |= blank & (t >> 7);

	//右上
	t = mask & (me << 9);

	t |= mask & (t << 9);
	t |= mask & (t << 9);
	t |= mask & (t << 9);
	t |= mask & (t << 9);
	t |= mask & (t << 9);

	mobility |= blank & (t << 9);

	//左上
	t = mask & (me << 7);

	t |= mask & (t << 7);
	t |= mask & (t << 7);
	t |= mask & (t << 7);
	t |= mask & (t << 7);
	t |= mask & (t << 7);

	mobility |= blank & (t << 7);

	return mobility;
}

//posに着手できるか
char BitBoard_CanFlip(const uint64 me, const uint64 ene, uint64 pos) {

	uint64 reverse = getReverseBits(&me, &ene, pos);

	if (reverse != 0) {
		return true;
	}

	return FALSE;
}

//反転数を返す
char BitBoard_CountFlips(const uint64 me, const uint64 ene, char pos) {

	uint64 reverse = getReverseBits(&me, &ene, pos);

	return BitBoard_CountStone(reverse);
}

void drawBits(uint64 bits) {
	int x;	
	printf(" 　　　　Ａ  Ｂ  Ｃ  Ｄ  Ｅ  Ｆ  Ｇ  Ｈ  \n");
	printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
	printf(" 　｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜\n");
	printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
	for (x = 0; x < BITBOARD_SIZE * BITBOARD_SIZE; x++, bits >>= 1) {
		if (x % 8 == 0) {
			printf(" %d ｜＃｜", x / 8 + 1);
		}
		if (bits & 1) {
			printf("●｜");
		}
		else {
			printf("　｜");
		}
		if (x % 8 == 7) {

			printf("＃｜ %d", x / 8 + 1);
			putchar('\n');
			printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
		}
	}
	printf(" 　｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜＃｜\n");
	printf(" 　＋―＋―＋―＋―＋―＋―＋―＋―＋―＋―＋\n");
	printf(" 　　　　Ａ  Ｂ  Ｃ  Ｄ  Ｅ  Ｆ  Ｇ  Ｈ\n");

}

//x,y座標からビットボード座標に変換
uint64 getBitPos(char x, char y) {
	return (uint64)1 << (x + y * BOARD_SIZE);
}

void getXY(uint64 pos, int *x, int *y) {
	pos = (-pos & pos) - 1;
	*y = BitBoard_CountStone(pos);
	*x = *y % 8;
	*y = *y / 8;
}

//colorの反対色(いらない？)
char oppColor(char color) {
	return WHITE + BLACK - color;
}

uint64 Stack_POP(BitBoard *bitboard) {
	return *(--bitboard->Sp);
}

void Stack_PUSH(BitBoard *bitboard, uint64 value) {
	*(bitboard->Sp++) = value;
}