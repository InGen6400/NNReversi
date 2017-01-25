
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BitBoard.h"
#include "const.h"
#include "Container.h"
#include "CPU.h"

//�{�[�h����
BitBoard *BitBoard_New(void) {
	BitBoard *res;
	res = (BitBoard*)malloc(sizeof(BitBoard));
	if (res) {
		BitBoard_Reset(res);
	}
	return res;
}

//�{�[�h�̍폜
void BitBoard_Delete(BitBoard *bitboard) {
	free(bitboard);
}

//�Ֆʂ̏�����
void BitBoard_Reset(BitBoard *bitboard) {
	bitboard->stone[WHITE] = 0x0000001008000000;//�^�񒆓�ȊO0
	bitboard->stone[BLACK] = 0x0000000810000000;//�^�񒆓�ȊO0
	//bitboard->stone[WHITE] = 0x007e7e7e6e7e7e00;//�^�񒆓�ȊO0
	//bitboard->stone[BLACK] = 0xFF818181818181FF;//�^�񒆓�ȊO0
}

//�Ֆʂ̕`��
void BitBoard_Draw(const BitBoard *bitboard, char isMob) {
	char x, y;

	BitBoard cpyBoard = *bitboard;

	uint64 mobility_w = BitBoard_getMobility(&cpyBoard, WHITE);
	uint64 mobility_b = BitBoard_getMobility(&cpyBoard, BLACK);

	printf("\n\n");
	printf(" �@�@�@�@�`  �a  �b  �c  �d  �e  �f  �g                        ��:%d  vs  ��:%d\n", BitBoard_CountStone(bitboard->stone[BLACK]), BitBoard_CountStone(bitboard->stone[WHITE]));
	printf(" �@�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{\n");
	printf(" �@�b���b���b���b���b���b���b���b���b���b���b\n");
	printf(" �@�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{\n");

	for (x = 0; x < BITBOARD_SIZE * BITBOARD_SIZE; x++, cpyBoard.stone[WHITE]>>=1, cpyBoard.stone[BLACK]>>=1, mobility_w >>= 1, mobility_b >>= 1) {
		if (x%8 == 0) {
			printf(" %d �b���b", x/8+1);
		}
		if (cpyBoard.stone[WHITE] & 1) {
			printf("���b");
		}
		else if (cpyBoard.stone[BLACK] & 1) {
			printf("�Z�b");
		}
		else if ((mobility_w & 1) && isMob) {
			printf("�D�b");
		}
		else if ((mobility_b & 1) && isMob) {
			printf("�B�b");
		}
		else {
			printf("�@�b");
		}
		if (x % 8 == 7) {

			printf("���b %d", x / 8+1);
			putchar('\n');
			printf(" �@�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{\n");
		}
	}
	printf(" �@�b���b���b���b���b���b���b���b���b���b���b\n");
	printf(" �@�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{\n");
	printf(" �@�@�@�@�`  �a  �b  �c  �d  �e  �f  �g\n");

}

//color�̐ΐ���Ԃ�(�����Ă���r�b�g����Ԃ�)
char BitBoard_CountStone(uint64 bits) {
	bits = bits - (bits>>1 & 0x5555555555555555);
	bits = (bits & 0x3333333333333333) + (bits >> 2 & 0x3333333333333333);
	bits = (bits & 0x0F0F0F0F0F0F0F0F) + (bits >> 4 & 0x0F0F0F0F0F0F0F0F);
	bits = (bits & 0x00FF00FF00FF00FF) + (bits >> 8 & 0x00FF00FF00FF00FF);
	bits = (bits & 0x0000FFFF0000FFFF) + (bits >> 16 & 0x0000FFFF0000FFFF);
	return (bits & 0x00000000FFFFFFFF) + (bits >> 32 & 0x00000000FFFFFFFF);
}

//pos�ɒ��肷��
char BitBoard_Flip(BitBoard *bitboard, char color, uint64 pos) {
	uint64 reverse;
	uint64 *me = &bitboard->stone[color];
	uint64 *ene = &bitboard->stone[(color+1)&1];

	reverse = getReverseBits(me, ene, pos);
	//drawBits(pos);

	if (reverse != 0) {
		*me ^= pos | reverse;
		*ene ^= reverse;
		return 1;
	}
	else {
		printf("iligal position\n");
	}

	return 0;
}

//���]����r�b�g��Ԃ�
inline uint64 getReverseBits(const uint64 *me, const uint64 *ene, const uint64 pos) {
	
	if (((*ene | *me) & pos) != 0)return 0;
	uint64 revBits = 0;
	const uint64 wh = *ene & 0x7E7E7E7E7E7E7E7E;
	const uint64 wv = *ene & 0x00FFFFFFFFFFFF00;

	//�E�T��6�}�X   
	revBits |= (pos >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3) | (*me << 4) | (*me << 5) | (*me << 6));
	revBits |= (pos >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3) | (*me << 4) | (*me << 5));
	revBits |= (pos >> 3) & (wh >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3) | (*me << 4));
	revBits |= (pos >> 4) & (wh >> 3) & (wh >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2) | (*me << 3));
	revBits |= (pos >> 5) & (wh >> 4) & (wh >> 3) & (wh >> 2) & (wh >> 1) & wh & ((*me << 1) | (*me << 2));
	revBits |= (pos >> 6) & (wh >> 5) & (wh >> 4) & (wh >> 3) & (wh >> 2) & (wh >> 1) & wh & (*me << 1);

	//���T��6�}�X
	revBits |= (pos >> 8) & wv & ((*me << 8) | (*me << 16) | (*me << 24) | (*me << 32) | (*me << 40) | (*me << 48));
	revBits |= (pos >> 16) & (wv >> 8) & wv & ((*me << 8) | (*me << 16) | (*me << 24) | (*me << 32) | (*me << 40));
	revBits |= (pos >> 24) & (wv >> 16) & (wv >> 8) & wv & ((*me << 8) | (*me << 16) | (*me << 24) | (*me << 32));
	revBits |= (pos >> 32) & (wv >> 24) & (wv >> 16) & (wv >> 8) & wv & ((*me << 8) | (*me << 16) | (*me << 24));
	revBits |= (pos >> 40) & (wv >> 32) & (wv >> 24) & (wv >> 16) & (wv >> 8) & wv & ((*me << 8) | (*me << 16));
	revBits |= (pos >> 48) & (wv >> 40) & (wv >> 32) & (wv >> 24) & (wv >> 16) & (wv >> 8) & wv & (*me << 8);

	//�E���T��6�}�X
	revBits |= (pos >> 9) & wh & ((*me << 9) | (*me << 18) | (*me << 27) | (*me << 36) | (*me << 45) | (*me << 54));
	revBits |= (pos >> 18) & (wh >> 9) & wh & ((*me << 9) | (*me << 18) | (*me << 27) | (*me << 36) | (*me << 45));
	revBits |= (pos >> 27) & (wh >> 18) & (wh >> 9) & wh & ((*me << 9) | (*me << 18) | (*me << 27) | (*me << 36));
	revBits |= (pos >> 36) & (wh >> 27) & (wh >> 18) & (wh >> 9) & wh & ((*me << 9) | (*me << 18) | (*me << 27));
	revBits |= (pos >> 45) & (wh >> 36) & (wh >> 27) & (wh >> 18) & (wh >> 9) & wh & ((*me << 9) | (*me << 18));
	revBits |= (pos >> 54) & (wh >> 45) & (wh >> 36) & (wh >> 27) & (wh >> 18) & (wh >> 9) & wh & (*me << 9);

	//�����T��6�}�X
	revBits |= (pos >> 7) & wh & ((*me << 7) | (*me << 14) | (*me << 21) | (*me << 28) | (*me << 35) | (*me << 42));
	revBits |= (pos >> 14) & (wh >> 7) & wh & ((*me << 7) | (*me << 14) | (*me << 21) | (*me << 28) | (*me << 35));
	revBits |= (pos >> 21) & (wh >> 14) & (wh >> 7) & wh & ((*me << 7) | (*me << 14) | (*me << 21) | (*me << 28));
	revBits |= (pos >> 28) & (wh >> 21) & (wh >> 14) & (wh >> 7) & wh & ((*me << 7) | (*me << 14) | (*me << 21));
	revBits |= (pos >> 35) & (wh >> 28) & (wh >> 21) & (wh >> 14) & (wh >> 7) & wh & ((*me << 7) | (*me << 14));
	revBits |= (pos >> 42) & (wh >> 35) & (wh >> 28) & (wh >> 21) & (wh >> 14) & (wh >> 7) & wh & (*me << 7);

	//���T��6�}�X
	revBits |= (pos << 1) & wh & ((*me >> 1) | (*me >> 2) | (*me >> 3) | (*me >> 4) | (*me >> 5) | (*me >> 6));
	revBits |= (pos << 2) & (wh << 1) & wh & ((*me >> 1) | (*me >> 2) | (*me >> 3) | (*me >> 4) | (*me >> 5));
	revBits |= (pos << 3) & (wh << 2) & (wh << 1) & wh & ((*me >> 1) | (*me >> 2) | (*me >> 3) | (*me >> 4));
	revBits |= (pos << 4) & (wh << 3) & (wh << 2) & (wh << 1) & wh & ((*me >> 1) | (*me >> 2) | (*me >> 3));
	revBits |= (pos << 5) & (wh << 4) & (wh << 3) & (wh << 2) & (wh << 1) & wh & ((*me >> 1) | (*me >> 2));
	revBits |= (pos << 6) & (wh << 5) & (wh << 4) & (wh << 3) & (wh << 2) & (wh << 1) & wh & (*me >> 1);

	//��T��6�}�X
	revBits |= (pos << 8) & wh & ((*me >> 8) | (*me >> 16) | (*me >> 24) | (*me >> 32) | (*me >> 40) | (*me >> 48));
	revBits |= (pos << 16) & (wh << 8) & wh & ((*me >> 8) | (*me >> 16) | (*me >> 24) | (*me >> 32) | (*me >> 40));
	revBits |= (pos << 24) & (wh << 16) & (wh << 8) & wh & ((*me >> 8) | (*me >> 16) | (*me >> 24) | (*me >> 32));
	revBits |= (pos << 32) & (wh << 24) & (wh << 16) & (wh << 8) & wh & ((*me >> 8) | (*me >> 16) | (*me >> 24));
	revBits |= (pos << 40) & (wh << 32) & (wh << 24) & (wh << 16) & (wh << 8) & wh & ((*me >> 8) | (*me >> 16));
	revBits |= (pos << 48) & (wh << 40) & (wh << 32) & (wh << 24) & (wh << 16) & (wh << 8) & wh & (*me >> 8);

	//�E��T��6�}�X
	revBits |= (pos << 7) & wh & ((*me >> 7) | (*me >> 14) | (*me >> 21) | (*me >> 28) | (*me >> 35) | (*me >> 42));
	revBits |= (pos << 14) & (wh << 7) & wh & ((*me >> 7) | (*me >> 14) | (*me >> 21) | (*me >> 28) | (*me >> 35));
	revBits |= (pos << 21) & (wh << 14) & (wh << 7) & wh & ((*me >> 7) | (*me >> 14) | (*me >> 21) | (*me >> 28));
	revBits |= (pos << 28) & (wh << 21) & (wh << 14) & (wh << 7) & wh & ((*me >> 7) | (*me >> 14) | (*me >> 21));
	revBits |= (pos << 35) & (wh << 28) & (wh << 21) & (wh << 14) & (wh << 7) & wh & ((*me >> 7) | (*me >> 14));
	revBits |= (pos << 42) & (wh << 35) & (wh << 28) & (wh << 21) & (wh << 14) & (wh << 7) & wh & (*me >> 7);

	//����T��6�}�X
	revBits |= (pos << 9) & wh & ((*me >> 9) | (*me >> 18) | (*me >> 27) | (*me >> 36) | (*me >> 45) | (*me >> 54));
	revBits |= (pos << 18) & (wh << 9) & wh & ((*me >> 9) | (*me >> 18) | (*me >> 27) | (*me >> 36) | (*me >> 45));
	revBits |= (pos << 27) & (wh << 18) & (wh << 9) & wh & ((*me >> 9) | (*me >> 18) | (*me >> 27) | (*me >> 36));
	revBits |= (pos << 36) & (wh << 27) & (wh << 18) & (wh << 9) & wh & ((*me >> 9) | (*me >> 18) | (*me >> 27));
	revBits |= (pos << 45) & (wh << 36) & (wh << 27) & (wh << 18) & (wh << 9) & wh & ((*me >> 9) | (*me >> 18));
	revBits |= (pos << 54) & (wh << 45) & (wh << 36) & (wh << 27) & (wh << 18) & (wh << 9) & wh & (*me >> 9);

	return revBits;
}

//���߂�
void BitBoard_Undo(BitBoard *bitboard) {
	char pos;
}

uint64 BitBoard_getMobility(BitBoard *bitboard, char color) {
	uint64 me = bitboard->stone[color];
	uint64 ene = bitboard->stone[(color + 1) & 1];

	uint64 blank = ~(me | ene);

	uint64 mobility;

	uint64 mask = ene & 0x7e7e7e7e7e7e7e7e;

	//��
	uint64 t = mask & (me << 1);

	t |= mask & (t << 1);
	t |= mask & (t << 1);
	t |= mask & (t << 1);
	t |= mask & (t << 1);
	t |= mask & (t << 1);

	mobility = blank & (t << 1);

	//�E
	t = mask & (me >> 1);

	t |= mask & (t >> 1);
	t |= mask & (t >> 1);
	t |= mask & (t >> 1);
	t |= mask & (t >> 1);
	t |= mask & (t >> 1);

	mobility |= blank & (t >> 1);

	//��
	t = mask & (me >> 8);

	t |= mask & (t >> 8);
	t |= mask & (t >> 8);
	t |= mask & (t >> 8);
	t |= mask & (t >> 8);
	t |= mask & (t >> 8);

	mobility |= blank & (t >> 8);

	//��
	t = mask & (me << 8);

	t |= mask & (t << 8);
	t |= mask & (t << 8);
	t |= mask & (t << 8);
	t |= mask & (t << 8);
	t |= mask & (t << 8);

	mobility |= blank & (t << 8);

	//�E��
	t = mask & (me >> 9);

	t |= mask & (t >> 9);
	t |= mask & (t >> 9);
	t |= mask & (t >> 9);
	t |= mask & (t >> 9);
	t |= mask & (t >> 9);

	mobility |= blank & (t >> 9);

	//����
	t = mask & (me >> 7);

	t |= mask & (t >> 7);
	t |= mask & (t >> 7);
	t |= mask & (t >> 7);
	t |= mask & (t >> 7);
	t |= mask & (t >> 7);

	mobility |= blank & (t >> 7);

	//�E��
	t = mask & (me << 9);

	t |= mask & (t << 9);
	t |= mask & (t << 9);
	t |= mask & (t << 9);
	t |= mask & (t << 9);
	t |= mask & (t << 9);

	mobility |= blank & (t << 9);

	//����
	t = mask & (me << 7);

	t |= mask & (t << 7);
	t |= mask & (t << 7);
	t |= mask & (t << 7);
	t |= mask & (t << 7);
	t |= mask & (t << 7);

	mobility |= blank & (t << 7);

	return mobility;
}

//pos�ɒ���ł��邩
char BitBoard_CanFlip(const uint64 me, const uint64 ene, char pos) {

	uint64 reverse = getReverseBits(&me, &ene, pos);

	if (reverse != 0) {
		return true;
	}

	return FALSE;
}

//���]����Ԃ�
char BitBoard_CountFlips(const BitBoard *bitboard, char color, char pos) {
	char flipCount = 0;


	return flipCount;
}

void drawBits(uint64 bits) {
	int x;	
	printf(" �@�@�@�@�`  �a  �b  �c  �d  �e  �f  �g  \n");
	printf(" �@�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{\n");
	printf(" �@�b���b���b���b���b���b���b���b���b���b���b\n");
	printf(" �@�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{\n");
	for (x = 0; x < BITBOARD_SIZE * BITBOARD_SIZE; x++, bits >>= 1) {
		if (x % 8 == 0) {
			printf(" %d �b���b", x / 8 + 1);
		}
		if (bits & 1) {
			printf("���b");
		}
		else {
			printf("�@�b");
		}
		if (x % 8 == 7) {

			printf("���b %d", x / 8 + 1);
			putchar('\n');
			printf(" �@�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{\n");
		}
	}
	printf(" �@�b���b���b���b���b���b���b���b���b���b���b\n");
	printf(" �@�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{\n");
	printf(" �@�@�@�@�`  �a  �b  �c  �d  �e  �f  �g\n");

}

//x,y���W����r�b�g�{�[�h���W�ɕϊ�
uint64 getBitPos(char x, char y) {
	return (uint64)1 << (x + y * BOARD_SIZE);
}

//color�̔��ΐF
char oppColor(char color) {
	return (color + 1) & 0x01;
}