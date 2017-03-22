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

static int Hive_Init(Hive *hive);

Hive *Hive_New();

void Hive_Del(Hive *hive);

uint64 NextMove(Hive *hive, const BitBoard *bitboard, int i_color, int *o_value);

int End_Search(Hive hive, uint64 me, uint64 opp, int depth, int color, int );

int Mid_Search();
