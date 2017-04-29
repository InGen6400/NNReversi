#pragma once

#include "Container.h"
#include "const.h"
#include <intrin.h>

#define WALL -1
#define NONE 2
#define WHITE 0
#define BLACK 1

#define PASS 0b11
#define NOMOVE 0b111

#define BITBOARD_SIZE 8

extern struct OpenKey;
typedef OpenKey OKey;

typedef enum
{
	ROT_NONE,
	ROT_R90,
	ROT_L90,
	ROT_180,
	ROT_LR,
	ROT_UD,
	ROT_DIAGA1,
	ROT_DIAGH1,
}RotateCode;

typedef unsigned long long uint64;

#pragma region POSITIONS

const char A1 = 63;
const char B1 = 62;
const char C1 = 61;
const char D1 = 60;
const char E1 = 59;
const char F1 = 58;
const char G1 = 57;
const char H1 = 56;

#pragma endregion


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

unsigned char BitBoard_CountStone(uint64 bits);

char BitBoard_Flip(BitBoard *bitboard, char color, uint64 pos);

int get_rand(int max);

void move_random(BitBoard *bitboard, char color);

uint64 getReverseBits(const uint64 *me, const uint64 *ene, const uint64 pos);

int BitBoard_Undo(BitBoard *bitboard);

uint64 BitBoard_getMobility(uint64 me, uint64 ene);

char BitBoard_CanFlip(const uint64 me, const uint64 ene, uint64 pos);

char BitBoard_CountFlips(const uint64 me, const uint64 ene, char pos);

void BitRotate128(uint64 *data1, uint64 *data2, RotateCode code);

OKey BitBoard_getKey(const BitBoard *board, char color);

void drawBits(uint64 bits);

uint64 getBitPos(char x, char y);

char getPos(char x, char y);

void getXY(uint64 pos, int *x, int *y);

uint64 getPos_book_upper(char *in);

uint64 getPos_book_lower(char *in);

char oppColor(char color);

uint64 Stack_POP(BitBoard *bitboard);

void Stack_PUSH(BitBoard *bitboard, uint64 value);
