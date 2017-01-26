#pragma once

#include "BitBoard.h"
#include "Board.h"

const char MAX_DEPTH = 10;

const int VALUE_MAX = 100000000;

const char poslist[] = {
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

struct CPU_ {
	int node;
	int start, end;
	BitBoard *bitboard;
	charNode isEmpty[BITBOARD_SIZE * BITBOARD_SIZE];
}typedef CPU;

CPU *CPU_Init(BitBoard *bitboard);

void CPU_PUT(CPU *cpu, uint64 *PutPos, char color, char left);

void CPU_Reset(CPU *cpu, BitBoard *bitboard);

int NegaAlphaSearch(uint64 me, uint64 ene, char isPassed, char color, char depth, uint64 *PutPos, int alpha);

int NegaEndSearch(uint64 me, uint64 ene, char isPassed, char color, char depth, uint64 *PutPos, int alpha);

int Evaluation(BitBoard *bitboard, char color);

void EmptyListInit(CPU *cpu, char color);