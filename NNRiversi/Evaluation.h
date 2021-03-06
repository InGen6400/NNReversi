#pragma once
#include "BitBoard.h"
#include "const.h"
#include <limits.h>

const char PATTERN_VALUE_FILE[] = "Resources\\PatternValue.bin";

const short TURN_SPLIT = 15;//15�i�K�ɕ����ĕ]���l��ς���
const double UPDATE_RATIO = 0.001;
const int MAX_VALUE = STONE_VALUE * 20;

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
	PATTERN_CORNER_STONE,
	PATTERN_X_STONE,
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
	128,//Stone Diff(-64 ~ 64)+64 -> (0~128)
	8,//CORNER_STONE(-4  ~ 4 )+4  -> (0~8)
	8,//X_STONE     (-4  ~ 4 )+4  -> (0~8)
	2,//Parity
	0,//Dummy(PatternAmount)
};

//�]���ɗ��p����e�p�^�[���̕]���l(���D����+)
static int *PatternValue[TURN_SPLIT+1][PATTERN_AMOUNT];
static unsigned short MirrorLine8[POW3_8];
static unsigned short MirrorLine7[POW3_7];
static unsigned short MirrorLine6[POW3_6];
static unsigned short MirrorLine5[POW3_5];
static unsigned short MirrorLine4[POW3_4];
static unsigned short MirrorCorner8[POW3_8];

void Pattern_Init();

void Pattern_Load();

int Pattern_Save();

void Pattern_setAVX();

int getValue(uint64 black, uint64 white, char left);

int getOrderValue(uint64 black, uint64 white, char left);

int getOrderValue_fast(uint64 black, uint64 white, char left);

void UpdateAllPattern(uint64 black, uint64 white, int value, char left);

void Patttern_Debug(int index);