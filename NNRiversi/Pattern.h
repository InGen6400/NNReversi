#pragma once
#include "BitBoard.h"

void Pattern_setAVX(unsigned char AVX2_FLAG);

unsigned short getCornerIndexUL(BitBoard *bitboard);

unsigned short getCornerIndexUR(BitBoard *bitboard);

unsigned short getCornerIndexDL(BitBoard *bitboard);

unsigned short getCornerIndexDR(BitBoard *bitboard);

unsigned short getLineIndex(BitBoard *bitboard, uint64 mask);

unsigned short getEdgeIndexUL_R(BitBoard *bitboard);

unsigned short getEdgeIndexUL_D(BitBoard *bitboard);

unsigned short getEdgeIndexUR_L(BitBoard *bitboard);

unsigned short getEdgeIndexUR_D(BitBoard *bitboard);

unsigned short getEdgeIndexDR_U(BitBoard *bitboard);

unsigned short getEdgeIndexDR_L(BitBoard *bitboard);