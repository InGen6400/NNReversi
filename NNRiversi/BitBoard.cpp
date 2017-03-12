
//2017.02.16  1405 +- 5

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BitBoard.h"
#include "const.h"
#include "Container.h"
#include "CPU.h"
#include "Flags.h"
#include <intrin.h>
#include <immintrin.h>

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
	//bitboard->stone[WHITE] = 0x0000000001020408;//
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
		*p = oppColor((char)*p);
		p--;//reverse‚Ö
		p--;//pos‚Ö(color‚Ìˆê‚Âæ)
	}
}

//”Õ–Ê‚Ì•`‰æ
void BitBoard_Draw(const BitBoard *bitboard, char isMob) {
	char x;

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
	return (char)_mm_popcnt_u64(bits);
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
	}/*
	else {
		printf("iligal position\n");
	}*/

	return 0;
}

//‰E‚©‚ç˜A‘±‚·‚éƒ[ƒ‚Ì”
inline int ntz(uint64 in) {
	return __popcnt64((in&(-in)) - 1);
}

inline __m256i nonzero(__m256i in) {
	return _mm256_add_epi64(_mm256_cmpeq_epi64(in, _mm256_setzero_si256()), _mm256_set1_epi64x(1));
}

inline __m128i h_or(__m256i in) {
	__m128i tmp = _mm_or_si128(_mm256_castsi256_si128(in), _mm256_extractf128_si256(in, 1));
	return _mm_or_si128(tmp, _mm_alignr_epi8(tmp, tmp, 8));
}

//”½“]‚·‚éƒrƒbƒg‚ğ•Ô‚·(—v‚‘¬‰»)
inline uint64 getReverseBits(const uint64 *me, const uint64 *opp, const uint64 pos) {

#ifdef __AVX2__	

#elif __AVX__

#endif
	
	__m256i mes = _mm256_set1_epi64x(*me);

	__m256i oppM = _mm256_and_si256(_mm256_set1_epi64x(*opp), _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x7E7E7E7E7E7E7E7E, 0x7E7E7E7E7E7E7E7E, 0x7E7E7E7E7E7E7E7E));

	__m256i mask = _mm256_set_epi64x(
		0x0080808080808080ULL,
		0x7F00000000000000ULL,
		0x0102040810204000ULL,
		0x0040201008040201ULL);
	int posCnt = ntz(pos);
	mask = _mm256_srli_epi64(mask, 63 - posCnt);


	uint64 AN[4];
	__m256i *andnot = (__m256i*)AN;
	*andnot = _mm256_andnot_si256(oppM, mask);
	__m256i outf = _mm256_set_epi64x(
		(0x8000000000000000ULL >> __lzcnt64(AN[3])) & *me,
		(0x8000000000000000ULL >> __lzcnt64(AN[2])) & *me,
		(0x8000000000000000ULL >> __lzcnt64(AN[1])) & *me,
		(0x8000000000000000ULL >> __lzcnt64(AN[0])) & *me
	);

	//flip = and(-outf << 1, mask)
	__m256i flip = _mm256_and_si256(_mm256_sub_epi64(_mm256_setzero_si256(), _mm256_slli_epi64(outf, 1)), mask);



	mask = _mm256_set_epi64x(
		0x0101010101010100ULL,
		0x00000000000000FEULL,
		0x0002040810204080ULL,
		0x8040201008040200ULL);

	mask = _mm256_slli_epi64(mask, posCnt);
	//outf = mask & ((oppM | ~mask) + 1) & mes
	outf = _mm256_and_si256(mask, _mm256_and_si256(_mm256_add_epi64(_mm256_andnot_si256(_mm256_or_si256(oppM, mask), _mm256_set1_epi64x(1)), _mm256_set1_epi64x(1)), mes));

	//flip = flip | ((outf - nonzero(outf)) & mask)
	flip = _mm256_or_si256(flip, _mm256_and_si256(mask, _mm256_sub_epi64(outf, nonzero(outf))));

	char posShift;
	__m128i rev = h_or(flip);
	return (uint64)(_mm_extract_epi64(rev, 0) | _mm_extract_epi64(rev, 1));
	
	/*
	uint64 revBits = 0;

	if (((*opp | *me) & pos) != 0)return 0;
	const uint64 wh = *opp & 0x007E7E7E7E7E7E00;
	const uint64 wv = *opp & 0x00FFFFFFFFFFFF00;

	//‰E’Tõ6ƒ}ƒX
	revBits |= (pos >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3) | (*me << 4) | (*me << 5) | (*me << 6));
	revBits |= (pos >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3) | (*me << 4) | (*me << 5));
	revBits |= (pos >> 3) & (wh >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3) | (*me << 4));
	revBits |= (pos >> 4) & (wh >> 3) & (wh >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3));
	revBits |= (pos >> 5) & (wh >> 4) & (wh >> 3) & (wh >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2));
	revBits |= (pos >> 6) & (wh >> 5) & (wh >> 4) & (wh >> 3) & (wh >> 2) & (wh >> 1) & wh & (*me << 1);

	//‰º’Tõ6ƒ}ƒX
	revBits |= (pos >> 8) & wh & ((*me << 8) | (*me << 16) | (*me << 24) | (*me << 32) | (*me << 40) | (*me << 48));
	revBits |= (pos >> 16) & (wh >> 8) & wh & ((*me << 8) | (*me << 16) | (*me << 24) | (*me << 32) | (*me << 40));
	revBits |= (pos >> 24) & (wh >> 16) & (wh >> 8) & wh & ((*me << 8) | (*me << 16) | (*me << 24) | (*me << 32));
	revBits |= (pos >> 32) & (wh >> 24) & (wh >> 16) & (wh >> 8) & wh & ((*me << 8) | (*me << 16) | (*me << 24));
	revBits |= (pos >> 40) & (wh >> 32) & (wh >> 24) & (wh >> 16) & (wh >> 8) & wh & ((*me << 8) | (*me << 16));
	revBits |= (pos >> 48) & (wh >> 40) & (wh >> 32) & (wh >> 24) & (wh >> 16) & (wh >> 8) & wh & (*me << 8);

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
	revBits |= (pos << 8) & wh & ((*me >> 8) | (*me >> 16) | (*me >> 24) | (*me >> 32) | (*me >> 40) | (*me >> 48));
	revBits |= (pos << 16) & (wh << 8) & wh & ((*me >> 8) | (*me >> 16) | (*me >> 24) | (*me >> 32) | (*me >> 40));
	revBits |= (pos << 24) & (wh << 16) & (wh << 8) & wh & ((*me >> 8) | (*me >> 16) | (*me >> 24) | (*me >> 32));
	revBits |= (pos << 32) & (wh << 24) & (wh << 16) & (wh << 8) & wh & ((*me >> 8) | (*me >> 16) | (*me >> 24));
	revBits |= (pos << 40) & (wh << 32) & (wh << 24) & (wh << 16) & (wh << 8) & wh & ((*me >> 8) | (*me >> 16));
	revBits |= (pos << 48) & (wh << 40) & (wh << 32) & (wh << 24) & (wh << 16) & (wh << 8) & wh & (*me >> 8);

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
	*/
}

//ˆêè–ß‚·(–¢À‘•)
int BitBoard_Undo(BitBoard *bitboard) {
	if (bitboard->Sp <= bitboard->Stack)return 0;
	char color = (char)Stack_POP(bitboard);
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

//Book‚ÌÀ•W(A1Œ`®)‚ğƒrƒbƒgƒ{[ƒhÀ•W‚É•ÏŠ·‚·‚é
uint64 getPos_book_upper(char *in) {
	return (uint64)1 << (('H'-in[0]) + ('8'-in[1])*8);
}

uint64 getPos_book_lower(char *in) {
	return (uint64)1 << (('h' - in[0]) + ('8' - in[1]) * 8);
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