#pragma once
#include "BitBoard.h"

struct _Hive
{
	BitBoard *bitboard;
	int openDepth;
	int midDepth;
	int endDepth;
	int Node;
}typedef Hive;

int Hive_Init(Hive *hive);

Hive *Hive_New();

void Hive_Del(Hive *hive);

void setLevel(Hive *hive, int mid, int end);

int NextMove(Hive *hive, const BitBoard *bitboard, char i_color, uint64 *PutPos);

int MidAlphaBeta(Hive *hive, uint64 me, uint64 opp, int alpha, int beta, char isPassed, char color, char left, char depth, uint64 *PutPos);

int EndAlphaBeta(Hive *hive, uint64 me, uint64 opp, int alpha, int beta, char isPassed, char color, char left, uint64 *PutPos);
