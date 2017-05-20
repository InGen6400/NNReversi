#include "MoveOrdering.h"
#include "Evaluation.h"
#include "BitBoard.h"
#include <stdio.h>

char posList_stat[64] = {
	//Šp
	0 , 7 , 56, 63,
	//Šp‚Ì2—×
	2 , 5 , 16, 23, 40, 47, 58, 61,
	//Šp‚ÌŽÎ‚ß2—×
	18, 21, 42, 45,
	//•Ó‚Ì’†S
	3 , 4 , 24, 31, 32, 39, 59, 60,
	//’†SŽü•Ó
	19, 20, 26, 29, 34, 37, 43, 44,

	10, 11, 12, 13,
	17, 22, 25, 30,
	33, 38, 41, 46,
	50, 51, 52, 53,
	1 , 6 , 8 , 15, 48, 55, 57, 62,
	9 , 14, 49, 54,
};

inline void swap(PElement *e1, PElement *e2) {
	PElement tmp;
	tmp = *e1;
	*e1 = *e2;
	*e2 = tmp;
}

void PList_Init(PList *list, uint64 black, uint64 white, char left) {
	uint64 mobility;
	mobility = BitBoard_getMobility(black, white);
	uint64 pos, rev;
	int i, j;
	list->count = 0;
	for (i = 0; i < 64; i++) {
		pos = 0x8000000000000000 >> posList_stat[i];
		if ((mobility & pos) != 0) {
			rev = getReverseBits(&black, &white, pos);
			list->element[list->count].value = -getOrderValue(white^rev, (black^rev) | pos, left);
			list->element[list->count].pos = posList_stat[i];
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
