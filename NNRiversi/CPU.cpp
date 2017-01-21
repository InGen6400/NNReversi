#include "stdafx.h"
#include "Board.h"
#include "const.h"
#include "CPU.h"
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

int NegaMaxSearch(CPU *cpu, char isPassed, char color, char depth, char *PutPos) {
	char x, y;
	int best=-VALUE_MAX, tmp;
	char move;

	if (depth >= MAX_DEPTH) {
		cpu->node++;
		return Evaluation(cpu->board, color);
	}

	for (y = 1; y <= BOARD_SIZE; y++) {
		for (x = 1; x <= BOARD_SIZE; x++) {
			if (Board_Flip(cpu->board, color, x, y)) {
				tmp = -NegaMaxSearch(cpu, FALSE, getOppStone(color), depth+1, &move);
				Board_Undo(cpu->board);
				if (best < tmp) {
					best = tmp;
					*PutPos = ConvertPos(x, y);
				}
			}
		}
	}

	if (best != -VALUE_MAX)return best;
	else if (isPassed == TRUE)return Evaluation(cpu->board, color);
	else {
		tmp = -NegaMaxSearch(cpu, TRUE, getOppStone(color), depth+1, &move);
		return tmp;
	}
	
}

int NegaAlphaSearch(CPU *cpu, char isPassed, char color, char depth, char *PutPos, int alpha) {
	char x, y;
	int best = -VALUE_MAX, tmp;
	char move;

	if (depth >= MAX_DEPTH) {
		cpu->node++;
		return Evaluation(cpu->board, color);
	}

	for (y = 1; y <= BOARD_SIZE; y++) {
		for (x = 1; x <= BOARD_SIZE; x++) {
			//後に空きマス表を作るので着手可能の判定は行わない
			if (Board_Flip(cpu->board, color, x, y)) {
				//再帰
				tmp = -NegaAlphaSearch(cpu, FALSE, getOppStone(color), depth + 1, &move, -best);
				Board_Undo(cpu->board);
				if (best < tmp) {
					best = tmp;
					*PutPos = ConvertPos(x, y);
				}
				//枝刈り
				if (best >= alpha)break;
			}
		}
	}

	if (best != -VALUE_MAX)return best;
	else if (isPassed == TRUE)return Evaluation(cpu->board, color);
	else {
		tmp = -NegaAlphaSearch(cpu, TRUE, getOppStone(color), depth + 1, &move, -best);
		return tmp;
	}
}

int Evaluation(Board *board, char color) {
	return Board_CountStone(board, color) - Board_CountStone(board, getOppStone(color));
}