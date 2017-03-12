#include "stdafx.h"
#include "BitBoard.h"
#include "const.h"
#include "CPU.h"
#include "Container.h"
#include <stdlib.h>
#include <immintrin.h>


CPU *CPU_Init() {
	CPU *ret;
	ret = (CPU*)malloc(sizeof(CPU));
	if (ret) {
		CPU_Reset(ret);
	}
	return ret;
}

void CPU_Reset(CPU *cpu) {
	cpu->bitboard = BitBoard_New();
	if (!cpu->bitboard) {
		printf("cpu->bitboard can't alloc");
		return;
	}
	cpu->leaf = 0;
}

void CPU_Delete(CPU *cpu) {
	if (cpu->bitboard != NULL) {
		BitBoard_Delete(cpu->bitboard);
	}
	free(cpu);
}

void CPU_Move(CPU *cpu, const BitBoard *in_board, uint64 *PutPos, char color, char left) {

	BitBoard_Copy(in_board, cpu->bitboard);
	EmptyListInit(cpu, color);
	if (left <= END_DEPTH) {
		printf("last point : %d", -NegaEndSearch(cpu->bitboard->stone[color], cpu->bitboard->stone[oppColor(color)], FALSE, color, END_DEPTH, PutPos, VALUE_MAX));
	}
	else {
		//���[�t�ŐF�����̂Ƃ�
		if ((color == WHITE && MID_DEPTH % 2 == 0) || (color == BLACK && MID_DEPTH % 2 == 1)) {
			//�{�[�h�𔒍����]
			BitBoard_AllOpp(cpu->bitboard);
			//���ɂ���
			color = oppColor(color);
		}
		printf("point : %d", NegaAlphaSearch(cpu->bitboard->stone[color], cpu->bitboard->stone[oppColor(color)], FALSE, color, MID_DEPTH, left, PutPos, VALUE_MAX));
	}
}

int NegaAlphaSearch(uint64 me, uint64 ene, char isPassed, char color, char depth, char left, uint64 *PutPos, int alpha) {
	//char x, y;
	int best = -VALUE_MAX, tmp;
	uint64 move;
	uint64 mobility;
	uint64 pos = 0;
	uint64 rev;

	if (depth <= 0) {
		return BitBoard_CountStone(me) - BitBoard_CountStone(ene);
	}

	mobility = BitBoard_getMobility(me, ene);
	while (mobility != 0) {
		//��ԉ��ɗ����Ă���r�b�g�𒅎�ʒu
		pos = ((-mobility) & mobility);
		//����\�ʒu�������
		mobility ^= pos;
		//���]����r�b�g���擾
		rev = getReverseBits(&me, &ene, pos);

		//�ċA
		tmp = -NegaAlphaSearch(ene ^ rev, me ^ rev ^ pos, FALSE, oppColor(color), depth - 1, left - 1, &move, -best);
		/*
		�����ɂĔ��]�����r�b�g�{�[�h��n�����ƂŒ�������Ƃɖ߂��Ȃ��Ă�����
		*/
		if (best < tmp) {
			best = tmp;
			*PutPos = pos;
		}
		//�}����
		if (best >= alpha)break;
	}
	if (best != -VALUE_MAX)return best;
	else if (isPassed == TRUE)return BitBoard_CountStone(me) - BitBoard_CountStone(ene);
	else {
		tmp = -NegaAlphaSearch(me, ene, TRUE, oppColor(color), depth - 1, left - 1, &move, -best);
		return tmp;
	}
}

int NegaEndSearch(uint64 me, uint64 ene, char isPassed, char color, char depth, uint64 *PutPos, int alpha) {
	//char x, y;
	int best = -VALUE_MAX, tmp;
	uint64 move;
	uint64 mobility;
	uint64 pos;
	uint64 rev;

	if (depth == 1) {
		pos = BitBoard_getMobility(me, ene);
		tmp = BitBoard_CountFlips(me, ene, pos);
		best = BitBoard_CountStone(me) - BitBoard_CountStone(ene);
		if (tmp > 0) {
			*PutPos = pos;
			return best + tmp + tmp + 1;
		}
		tmp = BitBoard_CountFlips(ene, me, pos);
		if (tmp > 0) {
			*PutPos = PASS;
			return best - tmp - tmp - 1;
		}
		*PutPos = NOMOVE;
		return best;
	}
	/*if (depth <= 0) {
	cpu->node++;
	return Evaluation(cpu->bitboard, color);
	}*/
	mobility = BitBoard_getMobility(me, ene);
	while (mobility != 0) {

		pos = ((-mobility) & mobility);
		mobility ^= pos;
		//�ċA
		rev = getReverseBits(&me, &ene, pos);

		tmp = -NegaEndSearch(ene ^ rev, me ^ rev ^ pos, FALSE, oppColor(color), depth - 1, &move, -best);

		if (best < tmp) {
			best = tmp;
			*PutPos = pos;
		}
		//�}����
		if (best >= alpha)break;
	}
	if (best != -VALUE_MAX)return best;
	else if (isPassed == TRUE)return BitBoard_CountStone(me) - BitBoard_CountStone(ene);
	else {
		tmp = -NegaEndSearch(me, ene, TRUE, oppColor(color), depth - 1, &move, -best);
		return tmp;
	}
}

int Evaluation(BitBoard *bitboard, char color) {
	return BitBoard_CountStone(bitboard->stone[color]) - BitBoard_CountStone(bitboard->stone[oppColor(color)]);
}

void EmptyListInit(CPU *cpu, char color) {
	int i = 0;
	charNode *node = cpu->isEmpty;
	node->value = NOMOVE;
	node->prev = NULL;
	node->next = NULL;
	for (i = 0; i < BITBOARD_SIZE*BITBOARD_SIZE; i++) {
		if (BitBoard_CanFlip(cpu->bitboard->stone[color], cpu->bitboard->stone[oppColor(color)],1<<i)) {
			node[1].value = poslist[i];
			node[1].prev = node;
			node[1].next = NULL;
			node->next = &node[1];
			node++;
		}
	}
}
