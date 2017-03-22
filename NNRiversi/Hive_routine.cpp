#include "stdafx.h"
#include "BitBoard.h"
#include "Hive_routine.h"

static int Hive_Init(Hive *hive) {
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

uint64 NextMove(Hive *hive, const BitBoard *bitboard, int i_color, int *o_value) {
	uint64 result;
	int left;
	int value;
	int color;

	BitBoard_Copy(bitboard, hive->bitboard);
	hive->Node = 0;
	left = 64 - (BitBoard_CountStone(BLACK) + BitBoard_CountStone(WHITE));
	//open‚Í–¢ì¬(ì¬—\’è)
	if (left <= hive->endDepth) {

	}
	else {

	}

	if (o_value) {
		*o_value = value;
	}

	return result;
}

