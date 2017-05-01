
//2017.02.16  1405 +- 5

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <intrin.h>
#include "BitBoard.h"
#include "bsTree.h"
#include "const.h"
#include "Container.h"
//#include "CPU.h"
#include "Flags.h"


#ifdef __AVX2__
alignas(64) __m256i flip_v8_table256;

#elif __AVX__
alignas(64) __m128i flip_v8_table128;
#endif

void Board_InitConst() {
#ifdef __AVX2__
	flip_v8_table256 = _mm256_set_epi8(
		24, 25, 26, 27, 28, 29, 30, 31,
		16, 17, 18, 19, 20, 21, 22, 23,
		8, 9, 10, 11, 12, 13, 14, 15,
		0, 1, 2, 3, 4, 5, 6, 7
	);
#elif __AVX__
	flip_v8_table128 = _mm_set_epi8(
		8, 9, 10, 11, 12, 13, 14, 15,
		0, 1, 2, 3, 4, 5, 6, 7
	);
#endif
}

class M256I {
public:
	__m256i m256i;
	M256I() {}
	M256I(__m256i in) : m256i(in) {}
	M256I(uint64 in) : m256i(_mm256_set1_epi64x(in)) {}
	M256I(uint64 x, uint64 y, uint64 z, uint64 w) : m256i(_mm256_set_epi64x(x, y, z, w)) {}

	M256I operator+() { return *this; }
	M256I operator-() { return _mm256_sub_epi64(_mm256_setzero_si256(), this->m256i); }
	M256I operator~() { return _mm256_andnot_si256(m256i, M256I(0xFFFFFFFFFFFFFFFFULL).m256i); }

	M256I operator+(const int in) { return _mm256_add_epi64(this->m256i, M256I(in).m256i); }
	M256I operator+(const M256I &in) { return _mm256_add_epi64(this->m256i, in.m256i); }

	M256I operator-(const int in) { return _mm256_sub_epi64(this->m256i, M256I(in).m256i); }
	M256I operator-(const M256I &in) { return _mm256_sub_epi64(this->m256i, in.m256i); }

	M256I operator >> (const int shift) { return _mm256_srli_epi64(this->m256i, shift); }
	M256I operator >> (const M256I shift) { return _mm256_srlv_epi64(this->m256i, shift.m256i); }
	M256I operator << (const int shift) { return _mm256_slli_epi64(this->m256i, shift); }
	M256I operator << (const M256I shift) { return _mm256_sllv_epi64(this->m256i, shift.m256i); }

	M256I operator| (const M256I &in) { return _mm256_or_si256(this->m256i, in.m256i); }
	M256I operator&(const M256I &in) { return _mm256_and_si256(this->m256i, in.m256i); }

};

inline uint64 sr(uint64 a, uint64 b) {
	return a >> b;
}

inline uint64 sl(uint64 a, uint64 b) {
	return a << b;
}
class M128I {
public:
	__m128i m128i;
	M128I() {}
	M128I(__m128i in) : m128i(in) {}
	M128I(uint64 in) : m128i(_mm_set1_epi64x(in)) {}
	M128I(uint64 x, uint64 y) : m128i(_mm_set_epi64x(x, y)) {}

	M128I operator+() { return *this; }
	M128I operator-() { return _mm_sub_epi64(_mm_setzero_si128(), this->m128i); }
	M128I operator~() { return _mm_andnot_si128(m128i, M128I(0xFFFFFFFFFFFFFFFFULL).m128i); }

	M128I operator+(const int in) { return _mm_add_epi64(this->m128i, M128I(in).m128i); }
	M128I operator+(const M128I &in) { return _mm_add_epi64(this->m128i, in.m128i); }

	M128I operator-(const int in) { return _mm_sub_epi64(this->m128i, M128I(in).m128i); }
	M128I operator-(const M128I &in) { return _mm_sub_epi64(this->m128i, in.m128i); }

	M128I operator >> (const int shift) { return _mm_srli_epi64(this->m128i, shift); }
	M128I operator >> (const M128I shift) {
		return _mm_set_epi64x(sr(_mm_extract_epi64(this->m128i, 1), (uint64)_mm_extract_epi64(shift.m128i, 1)), sr(_mm_extract_epi64(this->m128i, 0), (uint64)_mm_extract_epi64(shift.m128i, 0)));
	}
	M128I operator << (const int shift) { return _mm_slli_epi64(this->m128i, shift); }
	M128I operator << (const M128I shift) {
		return _mm_set_epi64x(sl(_mm_extract_epi64(this->m128i, 1), (uint64)_mm_extract_epi64(shift.m128i, 1)), sl(_mm_extract_epi64(this->m128i, 0), (uint64)_mm_extract_epi64(shift.m128i, 0)));
	}

	M128I operator| (const M128I &in) { return _mm_or_si128(this->m128i, in.m128i); }
	M128I operator& (const M128I &in) { return _mm_and_si128(this->m128i, in.m128i); }
	M128I operator^ (const M128I &in) { return _mm_xor_si128(this->m128i, in.m128i); }

};


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
	//bitboard->stone[WHITE] = 0x0000000810042840;//
	//bitboard->stone[BLACK] = 0x0124400000000000;//
	//Šp‚ÌƒCƒ“ƒfƒbƒNƒX‚ª22011021‚É‚È‚é‚Í‚¸
	bitboard->Sp = bitboard->Stack;
	Stack_PUSH(bitboard, 0xFFFFFFFFFFFFFFFFULL);
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
unsigned char BitBoard_CountStone(uint64 bits) {
	return (unsigned char)_mm_popcnt_u64(bits);
}

//pos‚É’…è‚·‚é
char BitBoard_Flip(BitBoard *bitboard, char color, uint64 pos) {
	uint64 reverse;
	uint64 *me = &bitboard->stone[color];
	uint64 *ene = &bitboard->stone[oppColor(color)];

	reverse = getReverseBits(me, ene, pos);

	//”½“]‰Â”\‚È‚ç’…è
	if (reverse != 0) {
		*me ^= pos | reverse;
		*ene ^= reverse;
		Stack_PUSH(bitboard, pos);
		Stack_PUSH(bitboard, reverse);
		Stack_PUSH(bitboard, color);
		return TRUE;
	}

	return FALSE;
}

int get_rand(int max)
{
	return (int)((double)max * rand() / (RAND_MAX + 1.0));
}

void move_random(BitBoard *bitboard, char color) {
	uint64 pos = 0;
	uint64 mobility = BitBoard_getMobility(bitboard->stone[color], bitboard->stone[oppColor(color)]);
	do {
		pos = (mobility & ((uint64)0x0000000000000001 << get_rand(64)));
	} while (pos == 0);
	//drawBits(pos);
	BitBoard_Flip(bitboard, color, pos);
}

//‰E‚©‚ç˜A‘±‚·‚éƒ[ƒ‚Ì”
inline unsigned long ntz(uint64 in) {
	unsigned long idx;
	if (_BitScanForward64(&idx, in)) {
		return idx;
	}
	else
	{
		return 64;
	}
}

inline M128I delta_swap(M128I bits, uint64 mask, unsigned char delta) {
	M128I x = (bits ^ (bits >> delta)) & mask;
	return bits ^ x ^ (x << delta);
}

#ifdef __AVX2__

//Flip vertical 8
inline M256I flipV8(M256I in) {
	return M256I(_mm256_shuffle_epi8(in.m256i, flip_v8_table256));
}

//in != 0
inline M256I nonzero(M256I in) {
	return _mm256_add_epi64(_mm256_cmpeq_epi64(in.m256i, _mm256_setzero_si256()), _mm256_set1_epi64x(1));
}

inline uint64 h_or(M256I in) {
	__m128i tmp = _mm_or_si128(_mm256_extractf128_si256(in.m256i, 0), _mm256_extractf128_si256(in.m256i, 1));
	return _mm_extract_epi64(tmp, 0) | _mm_extract_epi64(tmp, 1);
}


inline M256I andnot(const M256I in1, const M256I in2) {
	return _mm256_andnot_si256(in1.m256i, in2.m256i);
}

//leading zero count
inline M256I lzpos(M256I &in) {
	in = in | (in >> 1);
	in = in | (in >> 2);
	in = in | (in >> 4);
	in = andnot(in >> 1, in);
	in = flipV8(in);
	in = in & -in;
	return flipV8(in);
}
#elif __AVX__

inline M128I flipV8(M128I in) {
	return M128I(_mm_shuffle_epi8(in.m128i, flip_v8_table128));
}

inline M128I nonzero(M128I in) {
	return _mm_add_epi64(_mm_cmpeq_epi64(in.m128i, _mm_setzero_si128()), _mm_set1_epi64x(1));
}

//horizontal or
inline uint64 h_or(M128I in) {
	return _mm_extract_epi64(in.m128i, 0) | _mm_extract_epi64(in.m128i, 1);
}

inline M128I andnot(const M128I in1, const M128I in2) {
	return _mm_andnot_si128(in1.m128i, in2.m128i);
}

//leading zero count
inline M128I lzpos(M128I &in) {
	in = in | (in >> 1);
	in = in | (in >> 2);
	in = in | (in >> 4);
	in = andnot(in >> 1, in);
	in = flipV8(in);
	in = in & -in;
	return flipV8(in);
}

#endif


//”½“]‚·‚éƒrƒbƒg‚ğ•Ô‚·(—v‚‘¬‰»)
inline uint64 getReverseBits(const uint64 *me, const uint64 *opp, const uint64 pos) {
	//and
	//
#ifdef __AVX2__	
	//player's stones
	M256I mes = M256I(*me);
	//Masked Opp
	M256I oppM = M256I(0xFFFFFFFFFFFFFFFF, 0x7E7E7E7E7E7E7E7E, 0x7E7E7E7E7E7E7E7E, 0x7E7E7E7E7E7E7E7E) & M256I(*opp);

	int posCnt = ntz(pos);
	//mask for UP LEFT U_RIGHT U_LEFT
	M256I mask = M256I(
		0x0101010101010100ULL,
		0x00000000000000FEULL,
		0x0002040810204080ULL,
		0x8040201008040200ULL) << posCnt;
	//outflank
	M256I outf = mask & ((oppM | ~mask) + 1) & mes;
	//will flip
	M256I flip = (outf - nonzero(outf)) & mask;


	//DOWN RIGHT D_LEFT D_RIGHT
	mask = M256I(
		0x0080808080808080ULL,
		0x7F00000000000000ULL,
		0x0102040810204000ULL,
		0x0040201008040201ULL) >> (63 - posCnt);

	//outf = (MSB1 >> lzcnt(~oppM & mask)) & me
	//AVX2‚É‚Í•À—ñ‚ÅLZCNT‚·‚éˆ—‚ª‚È‚¢‚Ì‚Å”z—ñ‚É“WŠJ‚µ‚Älacnt()
	outf = lzpos(andnot(oppM, mask)) & mes;

	//flip = flip | ((-outf << 1) & mask)
	flip = flip | (-outf << 1) & mask;
	//horizontal or 64x4
	return h_or(flip);
#elif __AVX__

	//player's stones
	M128I mes = _mm_set1_epi64x(*me);
	//Masked Opp
	M128I oppM = M128I(0xFFFFFFFFFFFFFFFF, 0x7E7E7E7E7E7E7E7E) & M128I(*opp);

	int posCnt = ntz(pos);
	//mask for UP LEFT
	M128I mask = M128I(0x0101010101010100ULL, 0x00000000000000FEULL) << posCnt;

	//outflank
	M128I outf = mask & ((oppM | ~mask) + 1) & mes;
	//will flip
	M128I flip = (outf - nonzero(outf)) & mask;

	//DOWN RIGHT
	mask = M128I(0x0080808080808080ULL, 0x7F00000000000000ULL) >> (63 - posCnt);

	outf = lzpos(andnot(oppM, mask)) & mes;

	flip = flip | (-outf << 1) & mask;


	oppM = M128I(0x7E7E7E7E7E7E7E7EULL, 0x7E7E7E7E7E7E7E7EULL) & M128I(*opp);
	//mask for U_RIGHT U_LEFT
	mask = M128I(0x0002040810204080ULL, 0x8040201008040200ULL) << posCnt;

	outf = mask & ((oppM | ~mask) + 1) & mes;
	flip = flip | ((outf - nonzero(outf)) & mask);

	//D_LEFT D_RIGHT
	mask = M128I(0x0102040810204000ULL, 0x0040201008040201ULL) >> (63 - posCnt);

	outf = lzpos(andnot(oppM, mask)) & mes;

	flip = flip | (-outf << 1) & mask;

	//horizontal or 64x4
	return h_or(flip);
#endif
}

//ˆêè–ß‚·(–¢À‘•)
int BitBoard_Undo(BitBoard *bitboard) {
	if (*(bitboard->Sp - 1) == 0xFFFFFFFFFFFFFFFFULL)return 0;
	char color = (char)Stack_POP(bitboard);
	uint64 rev = Stack_POP(bitboard);
	bitboard->stone[color] ^= rev | Stack_POP(bitboard);
	bitboard->stone[oppColor(color)] ^= rev;
	return color;
}

#ifdef __AVX2__
uint64 BitBoard_getMobility(uint64 me, uint64 opp) {
	M256I mes(me);
	M256I oppM = M256I(0xFFFFFFFFFFFFFFFFULL, 0x7E7E7E7E7E7E7E7EULL, 0x7E7E7E7E7E7E7E7EULL, 0x7E7E7E7E7E7E7E7EULL) & M256I(opp);
	M256I empty = M256I(~(me | opp));
	M256I shift = M256I(8, 1, 9, 7);

	M256I move(oppM & (mes >> shift));
	move = move | (oppM & (move >> shift));
	move = move | (oppM & (move >> shift));
	move = move | (oppM & (move >> shift));
	move = move | (oppM & (move >> shift));
	move = move | (oppM & (move >> shift));
	M256I mobility = empty & (move >> shift);

	move = oppM & (mes << shift);
	move = move | (oppM & (move << shift));
	move = move | (oppM & (move << shift));
	move = move | (oppM & (move << shift));
	move = move | (oppM & (move << shift));
	move = move | (oppM & (move << shift));
	mobility = mobility | (empty & (move << shift));

	return h_or(mobility);
}

#elif __AVX__

uint64 BitBoard_getMobility(uint64 me, uint64 opp) {
	M128I mes(me);
	M128I empty = M128I(~(me | opp));
	M128I oppM = M128I(0xFFFFFFFFFFFFFFFFULL, 0x7E7E7E7E7E7E7E7EULL) & M128I(opp);
	M128I shift = M128I(8, 1);

	M128I move(oppM & (mes >> shift));
	move = move | (oppM & (move >> shift));
	move = move | (oppM & (move >> shift));
	move = move | (oppM & (move >> shift));
	move = move | (oppM & (move >> shift));
	move = move | (oppM & (move >> shift));
	M128I mobility = empty & (move >> shift);

	move = oppM & (mes << shift);
	move = move | (oppM & (move << shift));
	move = move | (oppM & (move << shift));
	move = move | (oppM & (move << shift));
	move = move | (oppM & (move << shift));
	move = move | (oppM & (move << shift));
	mobility = mobility | (empty & (move << shift));


	oppM = M128I(0x7E7E7E7E7E7E7E7EULL, 0x7E7E7E7E7E7E7E7EULL) & M128I(opp);
	shift = M128I(9, 7);

	move = (oppM & (mes >> shift));
	move = move | (oppM & (move >> shift));
	move = move | (oppM & (move >> shift));
	move = move | (oppM & (move >> shift));
	move = move | (oppM & (move >> shift));
	move = move | (oppM & (move >> shift));
	mobility = mobility | (empty & (move >> shift));

	move = oppM & (mes << shift);
	move = move | (oppM & (move << shift));
	move = move | (oppM & (move << shift));
	move = move | (oppM & (move << shift));
	move = move | (oppM & (move << shift));
	move = move | (oppM & (move << shift));
	mobility = mobility | (empty & (move << shift));

	return h_or(mobility);
}
#endif


//pos‚É’…è‚Å‚«‚é‚©
char BitBoard_CanFlip(const uint64 me, const uint64 ene, uint64 pos) {

	uint64 reverse = getReverseBits(&me, &ene, pos);

	if (reverse != 0) {
		return TRUE;
	}

	return FALSE;
}

//”½“]”‚ğ•Ô‚·
char BitBoard_CountFlips(const uint64 me, const uint64 ene, char pos) {

	uint64 reverse = getReverseBits(&me, &ene, pos);

	return BitBoard_CountStone(reverse);
}

inline M128I rot_LR(M128I in) {
	in = delta_swap(in, 0x5555555555555555, 1);
	in = delta_swap(in, 0x3333333333333333, 2);
	return delta_swap(in, 0x0F0F0F0F0F0F0F0F, 4);
}

inline M128I rot_UD(M128I in) {
	return _mm_shuffle_epi8(in.m128i, M128I(0x08090A0B0C0D0E0F, 0x0001020304050607).m128i);
}

inline M128I rot_diagA1(M128I in) {
	in = delta_swap(in, 0x00000000F0F0F0F0, 28);
	in = delta_swap(in, 0x0000CCCC0000CCCC, 14);
	return delta_swap(in, 0x00AA00AA00AA00AA, 7);
}

inline M128I rot_diagH1(M128I in) {
	in = delta_swap(in, 0x000000000F0F0F0F, 36);
	in = delta_swap(in, 0x0000333300003333, 18);
	return delta_swap(in, 0x0055005500550055, 9);
}

void BitRotate128(uint64 *data1, uint64 *data2, RotateCode code) {
	M128I out = _mm_set_epi64x(*data1, *data2);
	switch (code) {
	case ROT_180:
		out = rot_UD(rot_LR(out));
		break;
	case ROT_R90:
		out = rot_UD(rot_diagA1(out));
		break;
	case ROT_L90:
		out = rot_diagA1(rot_UD(out));
		break;
	case ROT_LR:
		out = rot_LR(out);
		break;
	case ROT_UD:
		out = rot_UD(out);
		break;
	case ROT_DIAGA1:
		out = rot_diagA1(out);
		break;
	case ROT_DIAGH1:
		out = rot_diagH1(out);
		break;
	default:
		return;
	}
	*data1 = _mm_extract_epi64(out.m128i, 1);
	*data2 = _mm_extract_epi64(out.m128i, 0);
}

void BitBoard_getKey(const BitBoard *board, char color, uint64 *retB, uint64 *retW) {
	char code;
	uint64 rotB, rotW;
	*retB = board->stone[oppColor(color)];
	*retW = board->stone[color];
	rotB = *retB;
	rotW = *retW;
	for (code = ROT_NONE; code <= ROT_DIAGH1; code++) {
		BitRotate128(retB, retW, (RotateCode)code);
		if (code == ROT_NONE || nodeKeyComp(retB, retW, &rotB, &rotW) > 0) {
			*retB = rotB;
			*retW = rotW;
		}
	}
}

void drawBits(const uint64 bits) {
	int x;	
	printf("\n\n");
	printf(" @@@@‚`  ‚a  ‚b  ‚c  ‚d  ‚e  ‚f  ‚g          \n");
	printf(" @{\{\{\{\{\{\{\{\{\{\{\n");
	printf(" @b”b”b”b”b”b”b”b”b”b”b\n");
	printf(" @{\{\{\{\{\{\{\{\{\{\{\n");

	for (x = BITBOARD_SIZE * BITBOARD_SIZE - 1; x >= 0; x--) {
		if ((x + 1) % 8 == 0) {
			printf(" %d b”b", 8 - x / 8);
		}
		if ((bits >> x) & 1) {
			printf("œb");
		}
		else if ((bits >> x) & 1) {
			printf("Zb");
		}
		else {
			printf("@b");
		}
		if ((x + 1) % 8 == 1) {

			printf("”b %d", 8 - x / 8);
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
	return (uint64)1 << (x + y * BITBOARD_SIZE);
}

char getPos(char x, char y) {
	return x + y * BITBOARD_SIZE;
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