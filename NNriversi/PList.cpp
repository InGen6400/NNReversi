#include "PList.h"
#include "Hive_routine.h"
#include "BitBoard.h"

void PList_Init(Hive *hive, PList *list, char color) {
	int i;
	int bottom = 0;
	uint64 mobility;
	mobility = BitBoard_getMobility(hive->bitboard->stone[color], hive->bitboard->stone[oppColor(color)]);
	for (i = 0; i < 64; i++) {
		if ((mobility & (A1_ >> i)) != 0) {
			list[bottom].pos = i;
			list[bottom].value = 0;
			bottom++;
		}
	}
	list[bottom].pos = -1;
}

void PList_Sort(Hive *hive, PList *list) {

}