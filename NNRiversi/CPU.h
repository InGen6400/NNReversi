#pragma once

const char MAX_DEPTH = 6;

const int VALUE_MAX = 100000000;

struct CPU_ {
	int node;
	int start, end;
	Board *board;
	charNode isEmpty[BOARD_SIZE * BOARD_SIZE];
}typedef CPU;

CPU *CPU_Init(Board *board);

void CPU_PUT(CPU *cpu, char *PutPos, char color);

void CPU_Reset(CPU *cpu, Board *board);

int NegaMaxSearch(CPU *cpu, char isPassed, char color, char depth, char *PutPos);

int NegaAlphaSearch(CPU *cpu, char isPassed, char color, char depth, char *PutPos, int alpha);

int Evaluation(Board *board, char color);

void EmptyListInit(CPU *cpu);