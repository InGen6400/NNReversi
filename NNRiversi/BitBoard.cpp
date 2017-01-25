
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
}

//�Ֆʂ̕`��
void BitBoard_Draw(const BitBoard *bitboard) {
	char x, y;

	BitBoard cpyBoard = *bitboard;

	printf("\n\n");
	printf(" �@�@�@�@�`  �a  �b  �c  �d  �e  �f  �g                        ��:  vs  ��:\n"/*, board->Black, board->White*/);
	printf(" �@�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{\n");
	printf(" �@�b���b���b���b���b���b���b���b���b���b���b\n");
	printf(" �@�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{�\�{\n");



	for (x = 0; x < BitBOARD_SIZE * BitBOARD_SIZE; x++, cpyBoard.stone[WHITE]<<=1, cpyBoard.stone[BLACK]<<=1) {
		if (x%8 == 0) {
			printf(" %d �b���b", x/8+1);
		}
		if (cpyBoard.stone[WHITE] & 0x8000000000000000) {
			printf("���b");
		}
		else if (cpyBoard.stone[BLACK] & 0x8000000000000000) {
			printf("�Z�b");
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

//color�̐ΐ���Ԃ�
char BitBoard_CountStone(uint64 bits) {
	bits = bits - (bits>>1 & 0x5555555555555555);
	bits = (bits & 0x3333333333333333) + (bits >> 2 & 0x3333333333333333);
	bits = (bits & 0x0F0F0F0F0F0F0F0F) + (bits >> 4 & 0x0F0F0F0F0F0F0F0F);
	bits = (bits & 0x00FF00FF00FF00FF) + (bits >> 8 & 0x00FF00FF00FF00FF);
	bits = (bits & 0x0000FFFF0000FFFF) + (bits >> 16 & 0x0000FFFF0000FFFF);
	return (bits & 0x00000000FFFFFFFF) + (bits >> 32 & 0x00000000FFFFFFFF);
}

//����
char BitBoard_Flip(uint64 *white, uint64 *black, uint64 pos) {
	uint64 reverse;
	

}

uint64 getReverseBits(const uint64 *white, const uint64 *black, const uint64 pos) {
	
	if (((*black | *white) & pos) != 0)return 0;
	uint64 revBits = 0;
	const uint64 wh = *white & 0x7E7E7E7E7E7E7E7E;

	revBits = (pos >> 1) & wh & ((*black << 1) | ((wh & (*black << 1)) << 1));

}

//�����𗠕Ԃ�
char BitBoard_FlipLine(BitBoard *bitboard, char color, char pos, char vec) {
	char oppColor = getOppStone(color);
	char flipCount = 0;

	pos += vec;

	return flipCount;
}

//���߂�
void BitBoard_Undo(BitBoard *bitboard) {
	char pos;
}

//����ł��邩
char BitBoard_CanPlay(BitBoard *bitboard, char color) {
	int i;

	return FALSE;
}

//pos�ɒ���ł��邩
char BitBoard_CanFlip(const BitBoard *bitboard, char color, char pos) {



	return FALSE;
}

//Board_CanFlip�̕⏕
char BitBoard_CanFlipLine(const BitBoard *bitboard, char color, char pos, char vec) {
	char oppColor = getOppStone(color);
	char count = 0;

	return count;
}

//���]����Ԃ�
char BitBoard_CountFlips(const BitBoard *bitboard, char color, char pos) {
	char flipCount = 0;


	return flipCount;
}

//x,y���W����r�b�g�{�[�h���W�ɕϊ�
uint64 getBitPos(char x, char y) {
	return (uint64)1 << (x + y * BOARD_SIZE);
}



