
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

//ƒ{[ƒh¶¬
BitBoard *BitBoard_New(void) {
	BitBoard *res;
	res = (BitBoard*)malloc(sizeof(BitBoard));
	if (res) {
		BitBoard_Reset(res);
	}
	return res;
}

//ƒ{[ƒh‚Ìíœ
void BitBoard_Delete(BitBoard *bitboard) {
	free(bitboard);
}

//”Õ–Ê‚Ì‰Šú‰»
void BitBoard_Reset(BitBoard *bitboard) {
	bitboard->stone[WHITE] = 0x0000001008000000;//^‚ñ’†“ñ‚ÂˆÈŠO0
	bitboard->stone[BLACK] = 0x0000000810000000;//^‚ñ’†“ñ‚ÂˆÈŠO00x0000000000000000
	bitboard->stone[WHITE] = 0x0000000001020408;//
	//bitboard->stone[BLACK] = 0x0000000000000000;//
	//Šp‚ÌƒCƒ“ƒfƒbƒNƒX‚ª22011021‚É‚È‚é‚Í‚¸
	bitboard->Sp = bitboard->Stack;
}

//”Õ–Ê‚ÌƒRƒs[
void BitBoard_Copy(const BitBoard *source, BitBoard *out) {
	*out = *source;
	out->Sp = source->Sp - source->Stack + out->Stack;
}

void BitBoard_AllOpp(BitBoard *source) {
	uint64 *p;
	uint64 stoneTmp;
	stoneTmp = source->stone[WHITE];
	source->stone[WHITE] = source->stone[BLACK];
	source->stone[BLACK] = stoneTmp;
	for (p = source->Sp; p > source->Stack ; ) {
		p--;//color‚Ö
		*p = oppColor(*p);
		p--;//reverse‚Ö
		p--;//pos‚Ö(color‚Ìˆê‚Âæ)
	}
}

//”Õ–Ê‚Ì•`‰æ
void BitBoard_Draw(const BitBoard *bitboard, char isMob) {
	char x, y;

	BitBoard cpyBoard = *bitboard;

	uint64 mobility_w = BitBoard_getMobility(cpyBoard.stone[WHITE], cpyBoard.stone[BLACK]);
	uint64 mobility_b = BitBoard_getMobility(cpyBoard.stone[BLACK], cpyBoard.stone[WHITE]);

	printf("\n\n");
	printf(" @@@@‚`  ‚a  ‚b  ‚c  ‚d  ‚e  ‚f  ‚g                        •:%d  vs  ”’:%d\n", BitBoard_CountStone(bitboard->stone[BLACK]), BitBoard_CountStone(bitboard->stone[WHITE]));
	printf(" @{\{\{\{\{\{\{\{\{\{\{\n");
	printf(" @b”b”b”b”b”b”b”b”b”b”b\n");
	printf(" @{\{\{\{\{\{\{\{\{\{\{\n");

	for (x = BITBOARD_SIZE * BITBOARD_SIZE - 1; x >= 0; x--) {
		if ((x+1)%8 == 0) {
			printf(" %d b”b", 8-x/8);
		}
		if ((cpyBoard.stone[WHITE]>>x) & 1) {
			printf("œb");
		}
		else if ((cpyBoard.stone[BLACK]>>x) & 1) {
			printf("Zb");
		}
		else if (((mobility_w>>x) & 1) && isMob) {
			printf("Db");
		}
		else if (((mobility_b>>x) & 1) && isMob) {
			printf("Bb");
		}
		else {
			printf("@b");
		}
		if ((x+1) % 8 == 1) {

			printf("”b %d", 8 - x / 8);
			putchar('\n');
			printf(" @{\{\{\{\{\{\{\{\{\{\{\n");
		}
	}
	printf(" @b”b”b”b”b”b”b”b”b”b”b\n");
	printf(" @{\{\{\{\{\{\{\{\{\{\{\n");
	printf(" @@@@‚`  ‚a  ‚b  ‚c  ‚d  ‚e  ‚f  ‚g\n");

}

//color‚ÌÎ”‚ğ•Ô‚·(—§‚Á‚Ä‚¢‚éƒrƒbƒg”‚ğ•Ô‚·)
char BitBoard_CountStone(uint64 bits) {
	/*bits = bits - (bits>>1 & 0x5555555555555555);
	bits = (bits & 0x3333333333333333) + (bits >> 2 & 0x3333333333333333);
	bits = (bits & 0x0F0F0F0F0F0F0F0F) + (bits >> 4 & 0x0F0F0F0F0F0F0F0F);
	bits = (bits & 0x00FF00FF00FF00FF) + (bits >> 8 & 0x00FF00FF00FF00FF);
	bits = (bits & 0x0000FFFF0000FFFF) + (bits >> 16 & 0x0000FFFF0000FFFF);
	return (bits & 0x00000000FFFFFFFF) + (bits >> 32 & 0x00000000FFFFFFFF);*/
	return _mm_popcnt_u64(bits);
}

//pos‚É’…è‚·‚é
char BitBoard_Flip(BitBoard *bitboard, char color, uint64 pos) {
	uint64 reverse;
	uint64 *me = &bitboard->stone[color];
	uint64 *ene = &bitboard->stone[(color+1)&1];

	reverse = getReverseBits(me, ene, pos);
	//drawBits(pos);

	//”½“]‰Â”\‚È‚ç’…è
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

//”½“]‚·‚éƒrƒbƒg‚ğ•Ô‚·(—v‚‘¬‰»)
inline uint64 getReverseBits(const uint64 *me, const uint64 *ene, const uint64 pos) {
	
	if (((*ene | *me) & pos) != 0)return 0;
	uint64 revBits = 0;
	const uint64 wh = *ene & 0x7E7E7E7E7E7E7E7E;
	const uint64 wv = *ene & 0x00FFFFFFFFFFFF00;

	//‰E’Tõ6ƒ}ƒX   
	revBits |= (pos >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3) | (*me << 4) | (*me << 5) | (*me << 6));
	revBits |= (pos >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3) | (*me << 4) | (*me << 5));
	revBits |= (pos >> 3) & (wh >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3) | (*me << 4));
	revBits |= (pos >> 4) & (wh >> 3) & (wh >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3));
	revBits |= (pos >> 5) & (wh >> 4) & (wh >> 3) & (wh >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2));
	revBits |= (pos >> 6) & (wh >> 5) & (wh >> 4) & (wh >> 3) & (wh >> 2) & (wh >> 1) & wh & (*me << 1);

	//‰º’Tõ6ƒ}ƒX
	revBits |= (pos >> 8) & wv & ((*me << 8) | (*me << 16) | (*me << 24) | (*me << 32) | (*me << 40) | (*me << 48));
	revBits |= (pos >> 16) & (wv >> 8) & wv & ((*me << 8) | (*me << 16) | (*me << 24) | (*me << 32) | (*me << 40));
	revBits |= (pos >> 24) & (wv >> 16) & (wv >> 8) & wv & ((*me << 8) | (*me << 16) | (*me << 24) | (*me << 32));
	revBits |= (pos >> 32) & (wv >> 24) & (wv >> 16) & (wv >> 8) & wv & ((*me << 8) | (*me << 16) | (*me << 24));
	revBits |= (pos >> 40) & (wv >> 32) & (wv >> 24) & (wv >> 16) & (wv >> 8) & wv & ((*me << 8) | (*me << 16));
	revBits |= (pos >> 48) & (wv >> 40) & (wv >> 32) & (wv >> 24) & (wv >> 16) & (wv >> 8) & wv & (*me << 8);

	//‰E‰º’Tõ6ƒ}ƒX
	revBits |= (pos >> 9) & wh & ((*me << 9) | (*me << 18) | (*me << 27) | (*me << 36) | (*me << 45) | (*me << 54));
	revBits |= (pos >> 18) & (wh >> 9) & wh & ((*me << 9) | (*me << 18) | (*me << 27) | (*me << 36) | (*me << 45));
	revBits |= (pos >> 27) & (wh >> 18) & (wh >> 9) & wh & ((*me << 9) | (*me << 18) | (*me << 27) | (*me << 36));
	revBits |= (pos >> 36) & (wh >> 27) & (wh >> 18) & (wh >> 9) & wh & ((*me << 9) | (*me << 18) | (*me << 27));
	revBits |= (pos >> 45) & (wh >> 36) & (wh >> 27) & (wh >> 18) & (wh >> 9) & wh & ((*me << 9) | (*me << 18));
	revBits |= (pos >> 54) & (wh >> 45) & (wh >> 36) & (wh >> 27) & (wh >> 18) & (wh >> 9) & wh & (*me << 9);

	//¶‰º’Tõ6ƒ}ƒX
	revBits |= (pos >> 7) & wh & ((*me << 7) | (*me << 14) | (*me << 21) | (*me << 28) | (*me << 35) | (*me << 42));
	revBits |= (pos >> 14) & (wh >> 7) & wh & ((*me << 7) | (*me << 14) | (*me << 21) | (*me << 28) | (*me << 35));
	revBits |= (pos >> 21) & (wh >> 14) & (wh >> 7) & wh & ((*me << 7) | (*me << 14) | (*me << 21) | (*me << 28));
	revBits |= (pos >> 28) & (wh >> 21) & (wh >> 14) & (wh >> 7) & wh & ((*me << 7) | (*me << 14) | (*me << 21));
	revBits |= (pos >> 35) & (wh >> 28) & (wh >> 21) & (wh >> 14) & (wh >> 7) & wh & ((*me << 7) | (*me << 14));
	revBits |= (pos >> 42) & (wh >> 35) & (wh >> 28) & (wh >> 21) & (wh >> 14) & (wh >> 7) & wh & (*me << 7);

	//¶’Tõ6ƒ}ƒX
	revBits |= (pos << 1) & wh & ((*me >> 1) | (*me >> 2) | (*me >> 3) | (*me >> 4) | (*me >> 5) | (*me >> 6));
	revBits |= (pos << 2) & (wh << 1) & wh & ((*me >> 1) | (*me >> 2) | (*me >> 3) | (*me >> 4) | (*me >> 5));
	revBits |= (pos << 3) & (wh << 2) & (wh << 1) & wh & ((*me >> 1) | (*me >> 2) | (*me >> 3) | (*me >> 4));
	revBits |= (pos << 4) & (wh << 3) & (wh << 2) & (wh << 1) & wh & ((*me >> 1) | (*me >> 2) | (*me >> 3));
	revBits |= (pos << 5) & (wh << 4) & (wh << 3) & (wh << 2) & (wh << 1) & wh & ((*me >> 1) | (*me >> 2));
	revBits |= (pos << 6) & (wh << 5) & (wh << 4) & (wh << 3) & (wh << 2) & (wh << 1) & wh & (*me >> 1);

	//ã’Tõ6ƒ}ƒX
	revBits |= (pos << 8) & wv & ((*me >> 8) | (*me >> 16) | (*me >> 24) | (*me >> 32) | (*me >> 40) | (*me >> 48));
	revBits |= (pos << 16) & (wv << 8) & wv & ((*me >> 8) | (*me >> 16) | (*me >> 24) | (*me >> 32) | (*me >> 40));
	revBits |= (pos << 24) & (wv << 16) & (wv << 8) & wv & ((*me >> 8) | (*me >> 16) | (*me >> 24) | (*me >> 32));
	revBits |= (pos << 32) & (wv << 24) & (wv << 16) & (wv << 8) & wv & ((*me >> 8) | (*me >> 16) | (*me >> 24));
	revBits |= (pos << 40) & (wv << 32) & (wv << 24) & (wv << 16) & (wv << 8) & wv & ((*me >> 8) | (*me >> 16));
	revBits |= (pos << 48) & (wv << 40) & (wv << 32) & (wv << 24) & (wv << 16) & (wv << 8) & wv & (*me >> 8);

	//‰Eã’Tõ6ƒ}ƒX
	revBits |= (pos << 7) & wh & ((*me >> 7) | (*me >> 14) | (*me >> 21) | (*me >> 28) | (*me >> 35) | (*me >> 42));
	revBits |= (pos << 14) & (wh << 7) & wh & ((*me >> 7) | (*me >> 14) | (*me >> 21) | (*me >> 28) | (*me >> 35));
	revBits |= (pos << 21) & (wh << 14) & (wh << 7) & wh & ((*me >> 7) | (*me >> 14) | (*me >> 21) | (*me >> 28));
	revBits |= (pos << 28) & (wh << 21) & (wh << 14) & (wh << 7) & wh & ((*me >> 7) | (*me >> 14) | (*me >> 21));
	revBits |= (pos << 35) & (wh << 28) & (wh << 21) & (wh << 14) & (wh << 7) & wh & ((*me >> 7) | (*me >> 14));
	revBits |= (pos << 42) & (wh << 35) & (wh << 28) & (wh << 21) & (wh << 14) & (wh << 7) & wh & (*me >> 7);

	//¶ã’Tõ6ƒ}ƒX
	revBits |= (pos << 9) & wh & ((*me >> 9) | (*me >> 18) | (*me >> 27) | (*me >> 36) | (*me >> 45) | (*me >> 54));
	revBits |= (pos << 18) & (wh << 9) & wh & ((*me >> 9) | (*me >> 18) | (*me >> 27) | (*me >> 36) | (*me >> 45));
	revBits |= (pos << 27) & (wh << 18) & (wh << 9) & wh & ((*me >> 9) | (*me >> 18) | (*me >> 27) | (*me >> 36));
	revBits |= (pos << 36) & (wh << 27) & (wh << 18) & (wh << 9) & wh & ((*me >> 9) | (*me >> 18) | (*me >> 27));
	revBits |= (pos << 45) & (wh << 36) & (wh << 27) & (wh << 18) & (wh << 9) & wh & ((*me >> 9) | (*me >> 18));
	revBits |= (pos << 54) & (wh << 45) & (wh << 36) & (wh << 27) & (wh << 18) & (wh << 9) & wh & (*me >> 9);

	return revBits;
}

//ˆêè–ß‚·(–¢À‘•)
int BitBoard_Undo(BitBoard *bitboard) {
	if (bitboard->Sp <= bitboard->Stack)return 0;
	char color = Stack_POP(bitboard);
	uint64 rev = Stack_POP(bitboard);
	bitboard->stone[color] ^= rev | Stack_POP(bitboard);
	bitboard->stone[oppColor(color)] ^= rev;
	return color;
}

//’…è‰Â”\ˆÊ’u(—v‚‘¬‰»)
uint64 BitBoard_getMobility(uint64 me, uint64 ene) {

	uint64 blank = ~(me | ene);

	uint64 mobility;

	uint64 mask = ene & 0x7e7e7e7e7e7e7e7e;

	//¶
	uint64 t = mask & (me << 1);

	t |= mask & (t << 1);
	t |= mask & (t << 1);
	t |= mask & (t << 1);
	t |= mask & (t << 1);
	t |= mask & (t << 1);

	mobility = blank & (t << 1);

	//‰E
	t = mask & (me >> 1);

	t |= mask & (t >> 1);
	t |= mask & (t >> 1);
	t |= mask & (t >> 1);
	t |= mask & (t >> 1);
	t |= mask & (t >> 1);

	mobility |= blank & (t >> 1);

	//‰º
	t = mask & (me >> 8);

	t |= mask & (t >> 8);
	t |= mask & (t >> 8);
	t |= mask & (t >> 8);
	t |= mask & (t >> 8);
	t |= mask & (t >> 8);

	mobility |= blank & (t >> 8);

	//ã
	t = mask & (me << 8);

	t |= mask & (t << 8);
	t |= mask & (t << 8);
	t |= mask & (t << 8);
	t |= mask & (t << 8);
	t |= mask & (t << 8);

	mobility |= blank & (t << 8);

	//‰E‰º
	t = mask & (me >> 9);

	t |= mask & (t >> 9);
	t |= mask & (t >> 9);
	t |= mask & (t >> 9);
	t |= mask & (t >> 9);
	t |= mask & (t >> 9);

	mobility |= blank & (t >> 9);

	//¶‰º
	t = mask & (me >> 7);

	t |= mask & (t >> 7);
	t |= mask & (t >> 7);
	t |= mask & (t >> 7);
	t |= mask & (t >> 7);
	t |= mask & (t >> 7);

	mobility |= blank & (t >> 7);

	//‰Eã
	t = mask & (me << 9);

	t |= mask & (t << 9);
	t |= mask & (t << 9);
	t |= mask & (t << 9);
	t |= mask & (t << 9);
	t |= mask & (t << 9);

	mobility |= blank & (t << 9);

	//¶ã
	t = mask & (me << 7);

	t |= mask & (t << 7);
	t |= mask & (t << 7);
	t |= mask & (t << 7);
	t |= mask & (t << 7);
	t |= mask & (t << 7);

	mobility |= blank & (t << 7);

	return mobility;
}

//pos‚É’…è‚Å‚«‚é‚©
char BitBoard_CanFlip(const uint64 me, const uint64 ene, uint64 pos) {

	uint64 reverse = getReverseBits(&me, &ene, pos);

	if (reverse != 0) {
		return true;
	}

	return FALSE;
}

//”½“]”‚ğ•Ô‚·
char BitBoard_CountFlips(const uint64 me, const uint64 ene, char pos) {

	uint64 reverse = getReverseBits(&me, &ene, pos);

	return BitBoard_CountStone(reverse);
}

void drawBits(uint64 bits) {
	int x;	
	printf(" @@@@‚`  ‚a  ‚b  ‚c  ‚d  ‚e  ‚f  ‚g  \n");
	printf(" @{\{\{\{\{\{\{\{\{\{\{\n");
	printf(" @b”b”b”b”b”b”b”b”b”b”b\n");
	printf(" @{\{\{\{\{\{\{\{\{\{\{\n");
	for (x = 0; x < BITBOARD_SIZE * BITBOARD_SIZE; x++, bits >>= 1) {
		if (x % 8 == 0) {
			printf(" %d b”b", x / 8 + 1);
		}
		if (bits & 1) {
			printf("œb");
		}
		else {
			printf("@b");
		}
		if (x % 8 == 7) {

			printf("”b %d", x / 8 + 1);
			putchar('\n');
			printf(" @{\{\{\{\{\{\{\{\{\{\{\n");
		}
	}
	printf(" @b”b”b”b”b”b”b”b”b”b”b\n");
	printf(" @{\{\{\{\{\{\{\{\{\{\{\n");
	printf(" @@@@‚`  ‚a  ‚b  ‚c  ‚d  ‚e  ‚f  ‚g\n");

}

//x,yÀ•W‚©‚çƒrƒbƒgƒ{[ƒhÀ•W‚É•ÏŠ·
uint64 getBitPos(char x, char y) {
	return (uint64)1 << (x + y * BOARD_SIZE);
}

void getXY(uint64 pos, int *x, int *y) {
	pos = (-pos & pos) - 1;
	*y = BitBoard_CountStone(pos);
	*x = *y % 8;
	*y = *y / 8;
}

//color‚Ì”½‘ÎF(‚¢‚ç‚È‚¢H)
char oppColor(char color) {
	return WHITE + BLACK - color;
}

uint64 Stack_POP(BitBoard *bitboard) {
	return *(--bitboard->Sp);
}

void Stack_PUSH(BitBoard *bitboard, uint64 value) {
	*(bitboard->Sp++) = value;
}