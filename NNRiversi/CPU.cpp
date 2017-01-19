#include "stdafx.h"
#include "Board.h"
#include "const.h"
#include "CPU.h"

unsigned int NegaMaxSearch(Board *board, char color, char depth, char *PutPos) {
	char x, y;
	unsigned int best=-1000000, tmp;

	if (depth >= MAX_DEPTH) {
		return Evaluation(board, color);
	}

	for (y = 1; y <= BOARD_SIZE; y++) {
		for (x = 1; x <= BOARD_SIZE; x++) {
			if (Board_CanFlip(board, color, x, y)) {
				Board_Flip(board, color, x, y);
				tmp = -NegaMaxSearch(board, getOppStone(color), depth+1, PutPos);
				Board_Undo(board);
				if (best < tmp) {
					best = tmp;
					*PutPos = ConvertPos(x, y);
				}
			}
		}
	}

	return best;
}

unsigned int Evaluation(Board *board, char color) {
	return Board_CountStone(board, color) - Board_CountStone(board, getOppStone(color));
}