#include "stdafx.h"
#include "BitBoard.h"
#include "Pattern.h"
#include "Hive_routine.h"

int Hive_Init(Hive *hive) {
	memset(hive, 0, sizeof(Hive));
	hive->bitboard = BitBoard_New();
	if (!hive->bitboard) {
		return 0;
	}
	hive->openDepth = 0;
	hive->midDepth = 0;
	hive->endDepth = 0;
	hive->Node = 0;
	return 1;
}

Hive *Hive_New() {
	Hive *hive;
	hive = (Hive*)malloc(sizeof(Hive));
	if (hive) {
		if (!Hive_Init(hive)) {
			Hive_Del(hive);
			hive = NULL;
		}
	}
	return hive;
}

void Hive_Del(Hive *hive) {
	if (hive->bitboard) {
		BitBoard_Delete(hive->bitboard);
	}
	free(hive);
}

void setLevel(Hive *hive, int mid, int end) {
	hive->midDepth = mid;
	hive->endDepth = end;
}

int NextMove(Hive *hive, const BitBoard *bitboard, char i_color, uint64 *PutPos) {
	char left;
	char color;

	BitBoard_Copy(bitboard, hive->bitboard);
	hive->Node = 0;
	left = 64 - (BitBoard_CountStone(bitboard->stone[BLACK]) + BitBoard_CountStone(bitboard->stone[WHITE]));
	//open‚Í–¢ì¬(ì¬—\’è)
	//I”Õ’Tõ
	if (left <= hive->endDepth) {
		return EndAlphaBeta(hive, hive->bitboard->stone[i_color], hive->bitboard->stone[oppColor(i_color)], -BITBOARD_SIZE * BITBOARD_SIZE, BITBOARD_SIZE*BITBOARD_SIZE,
			FALSE, i_color, left, PutPos);
	}
	else {//’†”Õ’Tõ
		if ((i_color == WHITE && hive->midDepth % 2 == 0) ||
			(i_color == BLACK && hive->midDepth % 2 == 1)) {

			BitBoard_AllOpp(hive->bitboard);
			color = oppColor(i_color);
		}
		else {
			color = i_color;
		}
		return ((double)MidAlphaBeta(hive, hive->bitboard->stone[color], hive->bitboard->stone[oppColor(color)], -VALUE_MAX, VALUE_MAX,
			FALSE, color, left, hive->midDepth, PutPos)+0.5)/STONE_VALUE;
	}
}

int MidAlphaBeta(Hive *hive, uint64 me, uint64 opp, int alpha, int beta, char isPassed, char color, char left, char depth, uint64 *PutPos) {
	int value, max = alpha;
	char moved = FALSE;
	uint64 move;
	uint64 mobility, pos, rev;

	if (depth == 0) {
		return getValue(me, opp, left);
	}
	*PutPos = NOMOVE;
	mobility = BitBoard_getMobility(me, opp);
	while (mobility != 0) {
		//mobility‚©‚çˆê‚Âæ‚èo‚·
		pos = ((-mobility) & mobility);
		mobility ^= pos;

		if (!moved) {
			*PutPos = pos;
			moved = TRUE;
		}

		rev = getReverseBits(&me, &opp, pos);
		value = -MidAlphaBeta(hive, opp^rev, (me^rev)|pos, -beta, -max, FALSE, oppColor(color), left-1, depth-1, &move);
		if (value > max) {
			max = value;
			*PutPos = pos;
			if (max >= beta) {
				return beta;
			}
		}
	}

	if (moved == FALSE) {
		if (isPassed == TRUE) {
			*PutPos = NOMOVE;
			max = (BitBoard_CountStone(me) - BitBoard_CountStone(opp)) * STONE_VALUE;
		}
		else {
			*PutPos = PASS;
			max = -MidAlphaBeta(hive, opp, me, -beta, -max, TRUE, oppColor(color), left, depth, &move);
		}
	}

	return max;
}

int EndAlphaBeta(Hive *hive, uint64 me, uint64 opp, int alpha, int beta, char isPassed, char color, char left, uint64 *PutPos) {
	int value, max = alpha;
	int moved = FALSE;
	uint64 move;
	uint64 mobility, pos, rev;

	if (left == 0) {
		return BitBoard_CountStone(me) - BitBoard_CountStone(opp);
	}

	*PutPos = NOMOVE;
	mobility = BitBoard_getMobility(me, opp);
	while (mobility != 0) {
		//mobility‚©‚çˆê‚Âæ‚èo‚·
		pos = ((-mobility) & mobility);
		mobility ^= pos;

		if (moved == FALSE) {
			*PutPos = pos;
			moved = TRUE;
		}

		rev = getReverseBits(&me, &opp, pos);
		value = -EndAlphaBeta(hive, opp^rev, (me^rev)|pos, -beta, -max, FALSE, oppColor(color), left - 1, &move);
		if (value > max) {
			max = value;
			*PutPos = pos;
			if (max >= beta) {
				return beta;
			}
		}
	}

	if (moved == FALSE) {
		if (isPassed == TRUE) {
			*PutPos = NOMOVE;
			max = BitBoard_CountStone(me) - BitBoard_CountStone(opp);
		}
		else {
			*PutPos = PASS;
			max = -EndAlphaBeta(hive, opp, me, -beta, -max, TRUE, oppColor(color), left, &move);
		}
	}

	return max;
}

