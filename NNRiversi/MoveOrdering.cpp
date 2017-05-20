#include "MoveOrdering.h"
#include "Evaluation.h"
#include "BitBoard.h"
#include <stdio.h>

inline void swap(PElement *e1, PElement *e2) {
	PElement tmp;
	tmp = *e1;
	*e1 = *e2;
	*e2 = tmp;
}

void PList_Init(PList *list, uint64 black, uint64 white) {
	uint64 mobility;
	mobility = BitBoard_getMobility(black, white);
	uint64 pos, rev;
	int i, j;
	list->count = 0;
	for (i = 0; i < 64; i++) {
		pos = 0x8000000000000000 >> i;
		if ((mobility & pos) != 0) {
			rev = getReverseBits(&black, &white, pos);
			list->element[list->count].value = -getOrderValue(white^rev, (black^rev) | pos, BitBoard_CountStone(~(black | white)));
			list->element[list->count].pos = i;
			j = list->count;
			while ((j > 0) && (list->element[j - 1].value < list->element[j].value)) {
				swap(&list->element[j - 1], &list->element[j]);
				j--;
			}
			list->count++;
		}
	}
}

void PList_Sort(PList *list) {
	int i, j;
	for (i = 0; i < list->count; i++) {
		j = i;
		while ((j > 0) && (list->element[j - 1].value < list->element[j].value)) {
			swap(&list->element[j - 1], &list->element[j]);
			j--;
		}
	}
}
