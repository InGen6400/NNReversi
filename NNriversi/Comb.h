#pragma once
#include "BitBoard.h"

#define COMBFILE "CombData.dat"

enum {
	COMB_LINE4,
	COMB_LINE3,
	COMB_LINE2,
	COMB_DIAG8,
	COMB_DIAG7,
	COMB_DIAG6,
	COMB_DIAG5,
	COMB_DIAG4,
	COMB_CORNER,
	COMB_EDGE,
	/*
	COMB_MOBILITY_B,
	COMB_MOBILITY_W,
	COMB_STONEDIFF,
	COMB_CORNER_STONE,
	COMB_X_STONE,
	*/
	COMB_PARITY,
	COMB_AMOUNT
};

static const short CombIndex[] =
{
	POW3_8,//Line
	POW3_8,//Line
	POW3_8,//Line
	POW3_8,//Diag8
	POW3_7,//Diag7
	POW3_6,//Diag6
	POW3_5,//Diag5
	POW3_4,//Diag4
	POW3_8,//Corner
	POW3_8,//Edge
	/*
	64,//Mobility_B
	64,//Mobility_W
	128,//Stone Diff(-64 ~ 64)+64 -> (0~128)
	8,//CORNER_STONE(-4  ~ 4 )+4  -> (0~8)
	8,//X_STONE     (-4  ~ 4 )+4  -> (0~8)
	*/
	2,//Parity
	0,//Dummy(PatternAmount)
};

static int *Value[16][COMB_AMOUNT];
static int MirrorLine_C[POW3_8];
static int MirrorCorner_C[POW3_8];

void Comb_Init();

void Comb_Delete();

int Comb_Load();

int Comb_Save();

int Comb_Value();

void Comb_Update();

void Comb_Debug(int index);