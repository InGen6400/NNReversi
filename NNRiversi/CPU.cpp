#include "stdafx.h"
#include "BitBoard.h"
#include "const.h"
#include "CPU.h"
#include "Container.h"
#include <stdlib.h>
#include <immintrin.h>

#define LEN 16

typedef unsigned short uint16;

CPU *CPU_Init(BitBoard *bitboard) {
	CPU *ret;
	ret = (CPU*)malloc(sizeof(CPU));
	if (ret) {
		CPU_Reset(ret, bitboard);
	}
	return ret;
}

void CPU_Reset(CPU *cpu, BitBoard *bitboard) {
	cpu->node = 0;
	cpu->bitboard = bitboard;
}

void CPU_PUT(CPU *cpu, uint64 *PutPos, char color, char left) {
	EmptyListInit(cpu, color);
	if (left <= 10) {
		NegaEndSearch(cpu->bitboard->stone[color], cpu->bitboard->stone[oppColor(color)], FALSE, color, MAX_DEPTH, PutPos, VALUE_MAX);
	}
	else {
		NegaAlphaSearch(cpu->bitboard->stone[color], cpu->bitboard->stone[oppColor(color)], FALSE, color, MAX_DEPTH, PutPos, VALUE_MAX);
	}
}

int NegaAlphaSearch(uint64 me, uint64 ene, char isPassed, char color, char depth, uint64 *PutPos, int alpha) {
	//char x, y;
	int best = -VALUE_MAX, tmp;
	uint64 move;
	uint64 mobility;
	uint64 pos = 0;
	uint64 rev;

	if (depth <= 0) {
		return BitBoard_CountStone(me) - BitBoard_CountStone(ene);
	}

	mobility = BitBoard_getMobility(me, ene);
	while (mobility != 0) {

		pos = ((-mobility) & mobility);
		mobility ^= pos;
		//再帰
		rev = getReverseBits(&me, &ene, pos);

		tmp = -NegaAlphaSearch(ene ^ rev, me ^ rev ^ pos, FALSE, oppColor(color), depth - 1, &move, -best);

		if (best < tmp) {
			best = tmp;
			*PutPos = pos;
		}
		//枝刈り
		if (best >= alpha)break;
	}
	if (best != -VALUE_MAX)return best;
	else if (isPassed == TRUE)return BitBoard_CountStone(me) - BitBoard_CountStone(ene);
	else {
		tmp = -NegaAlphaSearch(me, ene, TRUE, oppColor(color), depth - 1, &move, -best);
		return tmp;
	}
}

int NegaEndSearch(uint64 me, uint64 ene, char isPassed, char color, char depth, uint64 *PutPos, int alpha) {
	//char x, y;
	int best = -VALUE_MAX, tmp;
	uint64 move;
	uint64 mobility;
	uint64 pos;
	uint64 rev;

	if (depth == 1) {
		printf("last\n");
		pos = BitBoard_getMobility(me, ene);
		tmp = BitBoard_CountFlips(me, ene, pos);
		best = BitBoard_CountStone(me) - BitBoard_CountStone(ene);
		if (tmp > 0) {
			*PutPos = pos;
			return best + tmp + tmp + 1;
		}
		tmp = BitBoard_CountFlips(ene, me, pos);
		if (tmp > 0) {
			*PutPos = PASS;
			return best - tmp - tmp - 1;
		}
		*PutPos = NOMOVE;
		return best;
	}
	/*if (depth <= 0) {
	cpu->node++;
	return Evaluation(cpu->bitboard, color);
	}*/
	mobility = BitBoard_getMobility(me, ene);
	while (mobility != 0) {

		pos = ((-mobility) & mobility);
		mobility ^= pos;
		//再帰
		rev = getReverseBits(&me, &ene, pos);

		tmp = -NegaAlphaSearch(ene ^ rev, me ^ rev ^ pos, FALSE, oppColor(color), depth - 1, &move, -best);

		if (best < tmp) {
			best = tmp;
			*PutPos = pos;
		}
		//枝刈り
		if (best >= alpha)break;
	}
	if (best != -VALUE_MAX)return best;
	else if (isPassed == TRUE)return BitBoard_CountStone(me) - BitBoard_CountStone(ene);
	else {
		tmp = -NegaAlphaSearch(me, ene, TRUE, oppColor(color), depth - 1, &move, -best);
		return tmp;
	}
}

int Evaluation(BitBoard *bitboard, char color) {
	return BitBoard_CountStone(bitboard->stone[color]) - BitBoard_CountStone(bitboard->stone[oppColor(color)]);
}

void EmptyListInit(CPU *cpu, char color) {
	int i = 0;
	charNode *node = cpu->isEmpty;
	node->value = NOMOVE;
	node->prev = NULL;
	node->next = NULL;
	for (i = 0; i < BITBOARD_SIZE*BITBOARD_SIZE; i++) {
		if (BitBoard_CanFlip(cpu->bitboard->stone[color], cpu->bitboard->stone[oppColor(color)],1<<i)) {
			node[1].value = poslist[i];
			node[1].prev = node;
			node[1].next = NULL;
			node->next = &node[1];
			node++;
		}
	}
}

inline void setData(__m256i *ret, const unsigned char player, const unsigned char opp) {

#pragma region AVX2

	static const __m256i shifter = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
	__m256i mp = _mm256_set1_epi16(player);
	__m256i mo = _mm256_set1_epi16(opp);

	*ret = _mm256_unpackhi_epi16(mp, mo);

	*ret = _mm256_srlv_epi32(*ret, shifter);

	*ret = _mm256_and_si256(*ret, _mm256_set1_epi16(0x0001));

#pragma endregion

#pragma region AVX1
	/*
	//この部分はAVX2で高速化可能だがメインPCがAVX(1)なので断念
	*ret = _mm256_set_epi16(me >> 7, me >> 6, me >> 5, me >> 4, me >> 3, me >> 2, me >> 1, me,
	opp >> 7, opp >> 6, opp >> 5, opp >> 4, opp >> 3, opp >> 2, opp >> 1, opp);
	*ret = _mm256_and_si256(*ret, _mm256_set1_epi16(0x0001));
	*/
#pragma endregion
}

int getIndex(const unsigned char player, const unsigned char opp)
{

	alignas(16) static const uint16 pow_3[LEN] = { 0x1,  0x1 * 2,  0x3,  0x3 * 2,  0x9,   0x9 * 2,   0x1b,  0x1b * 2,
		0x51, 0x51 * 2, 0xf3, 0xf3 * 2, 0x2d9, 0x2d9 * 2, 0x88b, 0x88b * 2 };//(1,3,9,27,81,243,729,2187)*2 と1,3,9,27,81,243,729,2187
	alignas(16) uint16 y[LEN] = { 0 };
	alignas(16) uint16 z[LEN] = { 0 };

	//レジストリに登録
	__m256i *mmx = (__m256i *)pow_3;
	__m256i *mmy = (__m256i *)y;
	__m256i *mmz = (__m256i *)z;

	//データの整形
	setData(mmy, player, opp);

	//それぞれの積の和
	*mmz = _mm256_madd_epi16(*mmx, *mmy);
	//16bitx16bit=32bit
	*mmz = _mm256_hadd_epi32(*mmz, *mmz);
	*mmz = _mm256_hadd_epi32(*mmz, *mmz);
	//0バイト目と8バイト目がそれぞれの出力になる
	return z[0] + z[8];
}