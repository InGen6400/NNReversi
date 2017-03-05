#pragma once
#include "BitBoard.h"

void setAVX(char AVX2_FLAG);

short getCornerIndexUL(BitBoard *bitboard, char color);

short getCornerIndexUR(BitBoard *bitboard, char color);

short getCornerIndexDL(BitBoard *bitboard, char color);

short getCornerIndexDR(BitBoard *bitboard, char color);