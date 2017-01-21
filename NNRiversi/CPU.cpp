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

void CPU_PUT(CPU *cpu, char *PutPos, char color) {
	EmptyListInit(cpu);
	NegaAlphaSearch(cpu, FALSE, color, MAX_DEPTH, PutPos, VALUE_MAX);
	//NegaMaxSearch(cpu, FALSE, color, 0, PutPos);
}

int NegaMaxSearch(CPU *cpu, char isPassed, char color, char depth, char *PutPos) {
	char x, y;
	int best=-VALUE_MAX, tmp;
	char move;
	charNode *node;
	charNode *remNode;

	if (depth <= 0) {
		cpu->node++;
		return Evaluation(cpu->board, color);
	}
	/*
	for (y = 1; y <= BOARD_SIZE; y++) {
		for (x = 1; x <= BOARD_SIZE; x++) {
			if (Board_Flip(cpu->board, color, )) {
				tmp = -NegaMaxSearch(cpu, FALSE, getOppStone(color), depth-1, &move);
				Board_Undo(cpu->board);
				if (best < tmp) {
					best = tmp;
					*PutPos = ConvertPos(x, y);
				}
			}
		}
	}*/

	if (best != -VALUE_MAX)return best;
	else if (isPassed == TRUE)return Evaluation(cpu->board, color);
	else {
		tmp = -NegaMaxSearch(cpu, TRUE, getOppStone(color), depth-1, &move);
		return tmp;
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

int Evaluation(Board *board, char color) {
	return Board_CountStone(board, color) - Board_CountStone(board, getOppStone(color));
}


void EmptyListInit(CPU *cpu) {
	char poslist[] = {
		A1, A8, H8, H1,
		D3, D6, E3, E6, C4, C5, F4, F5,
		C3, C6, F3, F6,
		D2, D7, E2, E7, B4, B5, G4, G5,
		C2, C7, F2, F7, B3, B6, G3, G6,
		D1, D8, E1, E8, A4, A5, H4, H5,
		C1, C8, F1, F8, A3, A6, H3, H6,
		B2, B7, G2, G7,
		B1, B8, G1, G8, A2, A7, H2, H7,
		D4, D5, E4, E5,
		NOMOVE
	};
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