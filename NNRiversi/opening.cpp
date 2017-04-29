#include "opening.h"
#include "bsTree.h"
#include "BitBoard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static OPNode *OPTree;

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
		bsTree_add(OPTree ,&dbuf);
	}
	fclose(fp);
	return TRUE;
}

void open_read_text() {
	BitBoard *board;
	FILE *fp;
	char buf[OPFILE_LINE_SIZE];
	OPdata *opdata = NULL;
	int value=0, min;
	char history_move[BITBOARD_SIZE * BITBOARD_SIZE * 2];
	char color, turn;
	uint64 i;

	fp = fopen(OPEN_TEXT_NAME, "r");
	if (!fp) {
		printf("Can't open opening file@open_read_text\n");
		return;
	}
	board = BitBoard_New();
	//ファイル読み込みが終わったらbreak
	while (1) {
		BitBoard_Reset(board);
		color = BLACK;
		turn = 0;
		if (fgets(buf, sizeof(buf), fp) == NULL) {
			printf("finish Read opening Book@open_read_text\n");
			break;
		}

		strtok(buf, " ");
		value = (int)(atof(strtok(NULL, "\n")) * STONE_VALUE);

		for (i = 0; buf[i] != '\0' && buf[i + 1] != '\0'; i += 2) {
			if (BitBoard_getMobility(board->stone[color], board->stone[oppColor(color)]) == 0) {
				history_move[turn] = (char)PASS;
			}
			else {
				history_move[turn] = getPos(tolower(buf[i]) - 'a', buf[i+1] - '1');
				if (BitBoard_Flip(board, color, (uint64)0x8000000000000000 >> history_move[turn]) == FALSE) {
					printf("ERRRRRRRRRRRRRRRRRROR@open_read_text\n");
					break;
				}
			}
			turn++;
			color = oppColor(color);
			//printf("turn:%d\n", turn);
		}
		history_move[turn] = NOMOVE;
		
		for (; turn >= 0; turn--) {
			if (color == BLACK) {
				min = -value;
			}
			else {
				min = value;
			}
			
			if (history_move[turn] == PASS) {
				open_find(board, color, opdata);
				if (opdata != NULL) {
					min = opdata->value;
				}
			}
			else {
				for (i = 1; i != 0x8000000000000000; i <<= 1) {
					if (BitBoard_Flip(board, color, i) == TRUE) {
						open_find(board, color, opdata);
						if (opdata != NULL && opdata->value < min) {
							min = opdata->value;
						}
						BitBoard_Undo(board);
					}
				}
			}
			

			if (opdata == NULL) {
			}
			if (opdata != NULL) {
				opdata->value = -min;
			}
			else {
				printf("New Data Create Add\n");
				opdata = OPdata_New(board, color, value);
				bsTree_add(OPTree, opdata);
			}
			BitBoard_Undo(board);
			color = oppColor(color);
		}
		
	}
	fclose(fp);
	BitBoard_Delete(board);
	open_Save();
	bsTree_Delete(OPTree);
	printf("定石データ読み込み完了\n");
	return;
}

void open_find(const BitBoard *board, char color, OPdata *data) {
	OPdata dummyData;
	if (color == BLACK) {
		dummyData.bKey = board->stone[BLACK];
		dummyData.wKey = board->stone[WHITE];
	}
	else {
		dummyData.bKey = board->stone[WHITE];
		dummyData.wKey = board->stone[BLACK];
	}
	data = bsTreeSearch(OPTree ,&dummyData);
}