#include "stdafx.h"
#include "Board.h"
#include "const.h"
#include "CPU.h"
#include "Container.h"
#include <stdlib.h>


CPU *CPU_Init(Board *board) {
	CPU *ret;
	ret = (CPU*)malloc(sizeof(CPU));
	if (ret) {
		CPU_Reset(ret, board);
	}
	return ret;
}

void CPU_Reset(CPU *cpu, Board *board) {
	cpu->node = 0;
	cpu->board = board;
}

void CPU_PUT(CPU *cpu, char *PutPos, char color, char left) {
	EmptyListInit(cpu);
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
		return Evaluation(cpu->board, color);
	}
	for (node = cpu->isEmpty->next; node; node = node->next)
		if (Board_Flip(cpu->board, color, node->value)) {
			remNode = node;
			removeNode(remNode);
			//Ä‹A
			tmp = -NegaAlphaSearch(cpu, FALSE, getOppStone(color), depth - 1, &move, -best);
			Board_Undo(cpu->board);
			addNode(remNode);
			if (best < tmp) {
				best = tmp;
				*PutPos = node->value;
			}
			//Ž}Š ‚è
			if (best >= alpha)break;
		}
	if (best != -VALUE_MAX)return best;
	else if (isPassed == TRUE)return Evaluation(cpu->board, color);
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
		tmp = Board_CountFlips(cpu->board, color, node->value);
		best = Evaluation(cpu->board, color);
		if (tmp > 0) {
			*PutPos = node->value;
			return best + tmp + tmp + 1;
		}
		tmp = Board_CountFlips(cpu->board, getOppStone(color), cpu->isEmpty->next->value);
		if (tmp > 0) {
			*PutPos = PASS;
			return best - tmp - tmp - 1;
		}
		*PutPos = NOMOVE;
		return best;
	}
	/*if (depth <= 0) {
	cpu->node++;
	return Evaluation(cpu->board, color);
	}*/
	for (node = cpu->isEmpty->next; node; node = node->next)
		if (Board_Flip(cpu->board, color, node->value)) {
			remNode = node;
			removeNode(remNode);
			//Ä‹A
			tmp = -NegaAlphaSearch(cpu, FALSE, getOppStone(color), depth - 1, &move, -best);
			Board_Undo(cpu->board);
			addNode(remNode);
			if (best < tmp) {
				best = tmp;
				*PutPos = node->value;
			}
			//Ž}Š ‚è
			if (best >= alpha)break;
		}
	if (best != -VALUE_MAX)return best;
	else if (isPassed == TRUE)return Evaluation(cpu->board, color);
	else {
		tmp = -NegaAlphaSearch(cpu, TRUE, getOppStone(color), depth - 1, &move, -best);
		return tmp;
	}
}

int Evaluation(Board *board, char color) {
	return Board_CountStone(board, color) - Board_CountStone(board, getOppStone(color));
}


void EmptyListInit(CPU *cpu) {
	int i = 0;
	charNode *node = cpu->isEmpty;
	node->value = NOMOVE;
	node->prev = NULL;
	node->next = NULL;
	for (i = 0; i < BOARD_SIZE*BOARD_SIZE; i++) {
		if (cpu->board->Stone[poslist[i]] == NONE) {
			node[1].value = poslist[i];
			node[1].prev = node;
			node[1].next = NULL;
			node->next = &node[1];
			node++;
		}
	}
}