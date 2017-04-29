#include "opening.h"
#include "bsTree.h"
#include "BitBoard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static OPNode *OPTree = NULL;

char open_Save() {
	FILE *fp;
	fp = fopen(OPEN_BIN_NAME, "wb");
	if (fp == NULL) {
		printf("can't open opening data file: %s\n", OPEN_BIN_NAME);
		return FALSE;
	}
	bsTreeSave(fp, OPTree);
	fclose(fp);
	return TRUE;
}

char open_Load() {
	FILE *fp;
	OPdata dbuf;
	fp = fopen(OPEN_BIN_NAME, "rb");
	if (fp == NULL) {
		printf("can't open opening data file: %s\n", OPEN_BIN_NAME);
		return FALSE;
	}
	while (fread(&dbuf, sizeof(OPdata), 1, fp) == sizeof(OPdata)) {
		bsTree_add(&OPTree , &(dbuf.key), dbuf.value);
	}
	fclose(fp);
	return TRUE;
}

void open_read_text() {
	BitBoard *board = BitBoard_New();
	FILE *fp;
	char buf[OP_LINE_SIZE];
	char color, turn;
	int value, min;
	char history_move[BITBOARD_SIZE * BITBOARD_SIZE * 2];
	uint64 i;

	fp = fopen(OPEN_TEXT_NAME, "r");
	if (fp == NULL) {
		printf("Can't open Opening Data:%s\n", OPEN_TEXT_NAME);
		return;
	}

	while (1) {
		BitBoard_Reset(board);
		color = BLACK;
		turn = 0;

		if (fgets(buf, sizeof(buf), fp) == NULL) {
			break;
		}

		strtok(buf, " ");
		value = (int)(atof(strtok(buf, "\n")) * STONE_VALUE);

		/*ƒf[ƒ^‚ð“Ç‚Ýž‚ÝƒvƒŒƒC‚·‚é*/
		for (i = 0; buf[i] != '\0' && buf[i + 1] != '\0'; i += 2) {
			if (BitBoard_getMobility(board->stone[color], board->stone[oppColor(color)]) != 0) {
				history_move[turn] = PASS;
			}
			else {
				history_move[turn] = getPos(tolower(buf[i]) - 'a', buf[i+1] - '1');
				if (BitBoard_Flip(board, color, history_move[turn]) != FALSE) {
					break;
				}
			}
			turn++;
			color = oppColor(color);
		}
		history_move[turn] = NOMOVE;

		/*Šû•ˆ‚ð‹tÄ¶‚µ‚È‚ª‚çƒf[ƒ^‚ð“o˜^*/
		for (; turn >= 0; turn--) {
			if (color == BLACK) {
				min = -value;
			}
			else {
				min = value;
			}
			if (history_move[turn] == PASS) {
				if (bsTreeSearch(OPTree, &BitBoard_getKey(board, oppColor(color)), &min) == TRUE);
			}
			else {
				for (i = 1; i != 0x8000000000000000; i <<= 1) {
					if (BitBoard_Flip(board, color, i)) {
						if (bsTreeSearch(OPTree, &BitBoard_getKey(board, oppColor(color)), &min) == TRUE);
						BitBoard_Undo(board);
					}
				}
			}
			//printf("before:%d\n", -min);
			if (bsTree_add(&OPTree, &BitBoard_getKey(board, color), -min) == FALSE) {
				//printf("add\n");
			}
			//printf("after:%d\n", OPTree->data.value);
			//printf("after:%d\n", OPTree->data.value);
			BitBoard_Undo(board);
			color = oppColor(color);
		}
	}
	fclose(fp);
	BitBoard_Delete(board);
	open_Save();
	printf("’èÎ•ÏŠ·•“o˜^Š®—¹\n");
}

void open_find(const BitBoard *board, char color, OPdata *data) {                                                                                                  
	OPdata dummyData;
	if (color == BLACK) {
		dummyData.key.b = board->stone[BLACK];
		dummyData.key.w = board->stone[WHITE];
	}
	else {
		dummyData.key.b = board->stone[WHITE];
		dummyData.key.w = board->stone[BLACK];
	}
	//data = bsTreeSearch(OPTree ,&dummyData);
}