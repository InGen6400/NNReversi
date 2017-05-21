#pragma once
#include "BitBoard.h"

extern struct OpenHash;
typedef struct OpenHash OHash;

struct _Hive
{
	BitBoard *bitboard;
	OHash *opHash;
	char use_opening;
	char midDepth;
	char endDepth;
	int Node;
	int start, stop;
}typedef Hive;

int Hive_Init(Hive *hive);

Hive *Hive_New();

void Hive_Del(Hive *hive);

void setLevel(Hive *hive, char mid, char end, char use_opening);

int NextMove(Hive *hive, const BitBoard *bitboard, char i_color, uint64 *PutPos);

int OpeningSearch(Hive *hive, char color, uint64 *move);

int MidAlphaBeta(Hive *hive, uint64 me, uint64 opp, int alpha, int beta, char isPassed, char color, char left, char depth, uint64 *PutPos);

int EndAlphaBeta(Hive *hive, uint64 me, uint64 opp, int alpha, int beta, char isPassed, char color, char left, uint64 *PutPos);
