#pragma once

#include "Container.h"
#include "const.h"
/*
#pragma region POS_define

const char NOMOVE = -2;
const char PASS = -1;

const char A1 = 11;
const char A2 = 21;
const char A3 = 31;
const char A4 = 41;
const char A5 = 51;
const char A6 = 61;
const char A7 = 71;
const char A8 = 81;

const char B1 = 12;
const char B2 = 22;
const char B3 = 32;
const char B4 = 42;
const char B5 = 52;
const char B6 = 62;
const char B7 = 72;
const char B8 = 82;

const char C1 = 13;
const char C2 = 23;
const char C3 = 33;
const char C4 = 43;
const char C5 = 53;
const char C6 = 63;
const char C7 = 73;
const char C8 = 83;

const char D1 = 14;
const char D2 = 24;
const char D3 = 34;
const char D4 = 44;
const char D5 = 54;
const char D6 = 64;
const char D7 = 74;
const char D8 = 84;

const char E1 = 15;
const char E2 = 25;
const char E3 = 35;
const char E4 = 45;
const char E5 = 55;
const char E6 = 65;
const char E7 = 75;
const char E8 = 85;

const char F1 = 16;
const char F2 = 26;
const char F3 = 36;
const char F4 = 46;
const char F5 = 56;
const char F6 = 66;
const char F7 = 76;
const char F8 = 86;

const char G1 = 17;
const char G2 = 27;
const char G3 = 37;
const char G4 = 47;
const char G5 = 57;
const char G6 = 67;
const char G7 = 77;
const char G8 = 87;

const char H1 = 18;
const char H2 = 28;
const char H3 = 38;
const char H4 = 48;
const char H5 = 58;
const char H6 = 68;
const char H7 = 78;
const char H8 = 88;

#pragma endregion
*/
const char BITBOARD_SIZE = 8;

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
	uint64 Stack[BITBOARD_SIZE * BITBOARD_SIZE];
	uint64 *Sp;
}typedef BitBoard;

BitBoard *BitBoard_New(void);

void BitBoard_Delete(BitBoard *bitboard);

void BitBoard_Reset(BitBoard *bitboard);

void BitBoard_Draw(const BitBoard *bitboard, char isMob);

char BitBoard_CountStone(uint64 bits);

char BitBoard_Flip(BitBoard *bitboard, char color, uint64 pos);

uint64 getReverseBits(const uint64 *me, const uint64 *ene, const uint64 pos);

char BitBoard_FlipLine(BitBoard *bitboard, char color, char pos, char vec);

void BitBoard_Undo(BitBoard *bitboard);

uint64 BitBoard_getMobility(uint64 me, uint64 ene);

char BitBoard_CanFlip(const uint64 me, const uint64 ene, uint64 pos);

char BitBoard_CanFlipLine(const BitBoard *bitboard, char color, char pos, char vec);

char BitBoard_CountFlips(const uint64 me, const uint64 ene, char pos);

void drawBits(uint64 bits);

uint64 getBitPos(char x, char y);

char oppColor(char color);