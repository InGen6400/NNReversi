#include "stdafx.h"
#include "BitBoard.h"
#include "const.h"
#include "CPU.h"
#include "Container.h"
#include <stdlib.h>


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

void CPU_PUT(CPU *cpu, char *PutPos, char color, char left) {
	EmptyListInit(cpu, color);
	if (left <= 10) {
		NegaEndSearch(cpu, FALSE, color, left, PutPos, VALUE_MAX);
	}
	else {
		NegaAlphaSearch(cpu, FALSE, color, MAX_DEPTH, PutPos, VALUE_MAX);
	}
}

int NegaAlphaSearch(CPU *cpu, char isPassed, char color, char depth, char *PutPos, int alpha) {
	//char x, y;
	int best = -VALUE_MAX, tmp;
	char move;
	charNode *node;
	charNode *remNode;
	
	if (depth <= 0) {
		cpu->node++;
		return Evaluation(cpu->bitboard, color);
	}
	for (node = cpu->isEmpty->next; node; node = node->next)
		if (BitBoard_Flip(cpu->bitboard, color, node->value)) {
			remNode = node;
			removeNode(remNode);
			//Ä‹A
			tmp = -NegaAlphaSearch(cpu, FALSE, getOppStone(color), depth - 1, &move, -best);
			BitBoard_Undo(cpu->bitboard);
			addNode(remNode);
			if (best < tmp) {
				best = tmp;
				*PutPos = node->value;
			}
			//Ž}Š ‚è
			if (best >= alpha)break;
		}
	if (best != -VALUE_MAX)return best;
	else if (isPassed == TRUE)return Evaluation(cpu->bitboard, color);
	else {
		tmp = -NegaAlphaSearch(cpu, TRUE, getOppStone(color), depth - 1, &move, -best);
		return tmp;
	}
}

int NegaEndSearch(CPU *cpu, char isPassed, char color, char depth, char *PutPos, int alpha) {
	//char x, y;
	int best = -VALUE_MAX, tmp;
	char move;
	charNode *node;
	charNode *remNode;

	if (depth == 1) {
		printf("last\n");
		cpu->node++;
		node = cpu->isEmpty->next;
		tmp = BitBoard_CountFlips(cpu->bitboard, color, node->value);
		best = Evaluation(cpu->bitboard, color);
		if (tmp > 0) {
			*PutPos = node->value;
			return best + tmp + tmp + 1;
		}
		tmp = BitBoard_CountFlips(cpu->bitboard, getOppStone(color), cpu->isEmpty->next->value);
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
	for (node = cpu->isEmpty->next; node; node = node->next)
		if (BitBoard_Flip(cpu->bitboard, color, node->value)) {
			remNode = node;
			removeNode(remNode);
			//Ä‹A
			tmp = -NegaAlphaSearch(cpu, FALSE, getOppStone(color), depth - 1, &move, -best);
			BitBoard_Undo(cpu->bitboard);
			addNode(remNode);
			if (best < tmp) {
				best = tmp;
				*PutPos = node->value;
			}
			//Ž}Š ‚è
			if (best >= alpha)break;
		}
	if (best != -VALUE_MAX)return best;
	else if (isPassed == TRUE)return Evaluation(cpu->bitboard, color);
	else {
		tmp = -NegaAlphaSearch(cpu, TRUE, getOppStone(color), depth - 1, &move, -best);
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
		if (BitBoard_CanFlip(cpu->bitboard, color,1<<i)) {
			node[1].value = poslist[i];
			node[1].prev = node;
			node[1].next = NULL;
			node->next = &node[1];
			node++;
		}
	}
}