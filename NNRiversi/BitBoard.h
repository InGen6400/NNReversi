#pragma once

#include "Container.h"
#include "const.h"
#include <intrin.h>

const char WALL = -1;
const char NONE = 2;
const char WHITE = 0;
const char BLACK = 1;

const char BITBOARD_SIZE = 8;

typedef unsigned long long uint64;

struct _BitBoard
{
	//color index
	uint64 stone[2];
	uint64 Stack[BITBOARD_SIZE * BITBOARD_SIZE * 3];
	uint64 *Sp;
}typedef BitBoard;

void Board_InitConst();

void drawBits(const uint64 bits);

BitBoard *BitBoard_New(void);

void BitBoard_Delete(BitBoard *bitboard);

void BitBoard_Reset(BitBoard *bitboard);

void BitBoard_Copy(const BitBoard *source, BitBoard *out);

void BitBoard_AllOpp(BitBoard *source);

void BitBoard_Draw(const BitBoard *bitboard, char isMob);

char BitBoard_CountStone(uint64 bits);

char BitBoard_Flip(BitBoard *bitboard, char color, uint64 pos);

uint64 getReverseBits(const uint64 *me, const uint64 *ene, const uint64 pos);

int BitBoard_Undo(BitBoard *bitboard);

uint64 BitBoard_getMobility(uint64 me, uint64 ene);

char BitBoard_CanFlip(const uint64 me, const uint64 ene, uint64 pos);

char BitBoard_CountFlips(const uint64 me, const uint64 ene, char pos);

void drawBits(uint64 bits);

uint64 getBitPos(char x, char y);

void getXY(uint64 pos, int *x, int *y);

uint64 getPos_book_upper(char *in);

uint64 getPos_book_lower(char *in);

char oppColor(char color);

uint64 Stack_POP(BitBoard *bitboard);

void Stack_PUSH(BitBoard *bitboard, uint64 value);
