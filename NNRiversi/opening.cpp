#include "opening.h"
#include "bsTree.h"
#include "BitBoard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

char open_Save(OpenTree *tree) {
	FILE *fp;
	fp = fopen(OPEN_BIN_NAME, "wb");
	if (fp == NULL) {
		printf("can't open opening data file: %s\n", OPEN_BIN_NAME);
		return FALSE;
	}
	bsTreeSave(fp, tree->root);
	fclose(fp);
	return TRUE;
}

char open_Load(OpenTree *tree) {
	FILE *fp;
	OKey key;
	uint64 w,b;
	int value;
	
	fp = fopen(OPEN_BIN_NAME, "rb");
	if (fp == NULL) {
		printf("can't open opening data file: %s\n", OPEN_BIN_NAME);
		return FALSE;
	}
	printf("Load Opening Data...\n");
	while (fread(&b, sizeof(uint64), 1, fp) > 0) {
		fread(&w, sizeof(uint64), 1, fp);
		fread(&value, sizeof(int), 1, fp);
		key.b = b;
		key.w = w;
		bsTree_add(tree , &key, value);
	}
	printf("Finish loading\n");
	fclose(fp);
	return TRUE;
}

void open_read_text(OpenTree *tree) {
	BitBoard *board = BitBoard_New();
	FILE *fp;
	char buf[OP_LINE_SIZE];
	char color, turn;
	int value, min, searchResult;
	char history_move[BITBOARD_SIZE * BITBOARD_SIZE * 2];
	uint64 i;
	OKey key;

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
		value = (int)(atof(strtok(NULL, "\n")) * STONE_VALUE);
		/*ƒf[ƒ^‚ð“Ç‚Ýž‚ÝƒvƒŒƒC‚·‚é*/
		for (i = 0; buf[i] != '\0' && buf[i + 1] != '\0'; i += 2) {
			if (BitBoard_getMobility(board->stone[color], board->stone[oppColor(color)]) == 0) {
				history_move[turn] = PASS;
			}
			else {
				history_move[turn] = getPos(tolower(buf[i]) - 'a', buf[i+1] - '1');
				if (BitBoard_Flip(board, color, (uint64)0x8000000000000000 >> history_move[turn]) == FALSE) {
					printf("Can't put opening:%dl\n", __LINE__);
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
				BitBoard_getKey(board, oppColor(color), &key.b, &key.w);
				bsTreeSearch(tree, &key, &min);
			}
			else {
				for (i = 1; i != 0x8000000000000000; i <<= 1) {
					if (BitBoard_Flip(board, color, i) == TRUE) {
						BitBoard_getKey(board, oppColor(color), &key.b, &key.w);
						bsTreeSearch(tree, &key, &searchResult);
						if (searchResult < min) {
							min = searchResult;
						}
						BitBoard_Undo(board);
					}
				}
			}
			BitBoard_getKey(board, color, &key.b, &key.w);
			bsTree_add(tree, &key, -min);
			BitBoard_Undo(board);
			color = oppColor(color);
		}
	}
	fclose(fp);
	BitBoard_Delete(board);
	open_Save(tree);
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