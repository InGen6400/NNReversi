#include "stdafx.h"
#include "BitBoard.h"
#include "const.h"
#include "CPU.h"
#include "Container.h"
#include <stdlib.h>
#include <immintrin.h>



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
		//çƒãA
		rev = getReverseBits(&me, &ene, pos);

		tmp = -NegaAlphaSearch(ene ^ rev, me ^ rev ^ pos, FALSE, oppColor(color), depth - 1, &move, -best);

		if (best < tmp) {
			best = tmp;
			*PutPos = pos;
		}
		//é}ä†ÇË
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
		//çƒãA
		rev = getReverseBits(&me, &ene, pos);

		tmp = -NegaAlphaSearch(ene ^ rev, me ^ rev ^ pos, FALSE, oppColor(color), depth - 1, &move, -best);

		if (best < tmp) {
			best = tmp;
			*PutPos = pos;
		}
		//é}ä†ÇË
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
