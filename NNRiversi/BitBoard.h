#pragma once

#include "Container.h"
#include "const.h"

const char BitBOARD_SIZE = 8;

typedef unsigned long long uint64;

struct bitFlipData
{
	char filpCount;
	char flipPos[6];
}typedef bitFlipData;

struct _BitBoard
{
	//color index
	uint64 stone[2];
	uint64 Stack[BitBOARD_SIZE * BitBOARD_SIZE];
	uint64 *Sp;
}typedef BitBoard;

BitBoard *BitBoard_New(void);

void BitBoard_Delete(BitBoard *bitboard);

void BitBoard_Reset(BitBoard *bitboard);

void BitBoard_Draw(const BitBoard *bitboard);

char BitBoard_CountStone(uint64 bits);

char BitBoard_Flip(BitBoard *bitboard, char color, char pos);

char BitBoard_FlipLine(BitBoard *bitboard, char color, char pos, char vec);

void BitBoard_Undo(BitBoard *bitboard);

char BitBoard_CanPlay(BitBoard *bitboard, char color);

char BitBoard_CanFlip(const BitBoard *bitboard, char color, char pos);

char BitBoard_CanFlipLine(const BitBoard *bitboard, char color, char pos, char vec);

char BitBoard_CountFlips(const BitBoard *bitboard, char color, char pos);

uint64 getBitPos(char x, char y);