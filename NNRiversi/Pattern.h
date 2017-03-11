#pragma once
#include "BitBoard.h"
#include "const.h"

const char PATTERN_VALUE_FILE[] = "PatternValue.dat";

const short TURN_SPLIT = 15;//15段階に分けて評価値を変える
const double UPDATE_RATIO = 0.002;

enum {
	PATTERN_LINE4,
	PATTERN_LINE3,
	PATTERN_LINE2,
	PATTERN_DIAG8,
	PATTERN_DIAG7,
	PATTERN_DIAG6,
	PATTERN_DIAG5,
	PATTERN_DIAG4,
	PATTERN_CORNER,
	PATTERN_EDGE,
	PATTERN_MOBILITY_B,
	PATTERN_MOBILITY_W,
	PATTERN_STONEDIFF,
	PATTERN_PARITY,
	PATTERN_AMOUNT
};

static const short PatternIndex[] =
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
	64,//Mobility_B
	64,//Mobility_W
	64,//Stone Diff
	2,//Parity
	0,//Dummy(PatternAmount)
};

//評価に利用する各パターンの評価値(黒優勢で+)
static int *PatternValue[TURN_SPLIT][PATTERN_AMOUNT];

void Pattern_Load();

void Pattern_Save();

void Pattern_setAVX(unsigned char AVX2_FLAG);

void UpdateAllPattern(const BitBoard *board, int value, char turn);
