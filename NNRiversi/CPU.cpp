#include "stdafx.h"
#include "Board.h"
#include "const.h"
#include "CPU.h"

int NegaMaxSearch(Board *board, char isPassed, char color, char depth, char *PutPos) {
	char x, y;
	int best=-1000000, tmp;
	char move;

	if (depth >= MAX_DEPTH) {
		return Evaluation(board, color);
	}

	for (y = 1; y <= BOARD_SIZE; y++) {
		for (x = 1; x <= BOARD_SIZE; x++) {
			if (Board_CanFlip(board, color, x, y)) {
				Board_Flip(board, color, x, y);
				tmp = -NegaMaxSearch(board, FALSE, getOppStone(color), depth+1, &move);
				Board_Undo(board);
				if (best < tmp) {
					best = tmp;
					*PutPos = ConvertPos(x, y);
				}
			}
		}
	}

	if (best != -1000000)return best;
	else if (isPassed == TRUE)return Evaluation(board, color);
	else {
		tmp = NegaMaxSearch(board, TRUE, getOppStone(color), depth+1, &move);
		return tmp;
	}
	
}

int Evaluation(Board *board, char color) {
	return Board_CountStone(board, color) - Board_CountStone(board, getOppStone(color));
}