#include "opening.h"
#include "BitBoard.h"
#include "OHash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

char open_Save(OHash *hash) {
	FILE *fp;
	fp = fopen(OPEN_BIN_NAME, "wb");
	if (fp == NULL) {
		printf("can't open opening data file: %s\n", OPEN_BIN_NAME);
		return FALSE;
	}
	OHash_Save(hash, fp);
	fclose(fp);
	return TRUE;
}

char open_Load(OHash *hash) {
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
		OHash_Add(hash , &key, value);
	}
	printf("Finish loading\n");
	fclose(fp);
	return TRUE;
}

void open_read_text(OHash *opHash) {
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
		/*データを読み込みプレイする*/
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

		/*棋譜を逆再生しながらデータを登録*/
		for (; turn >= 0; turn--) {
			if (color == BLACK) {
				min = -value;
			}
			else {
				min = value;
			}
			if (history_move[turn] == PASS) {
				BitBoard_getKey(board, oppColor(color), &key.b, &key.w);
				OHash_Search(opHash, &key, &min);
			}
			else {
				for (i = 1; i != 0x8000000000000000; i <<= 1) {
					if (BitBoard_Flip(board, color, i) == TRUE) {
						BitBoard_getKey(board, oppColor(color), &key.b, &key.w);
						if (OHash_Search(opHash, &key, &searchResult) == TRUE) {
							if (searchResult < min) {
								min = searchResult;
							}
						}
						BitBoard_Undo(board);
					}
				}
			}
			BitBoard_getKey(board, color, &key.b, &key.w);
			OHash_Add(opHash, &key, -min);
			BitBoard_Undo(board);
			color = oppColor(color);
		}
	}
	fclose(fp);
	BitBoard_Delete(board);
	open_Save(opHash);
	printf("collide:%d count:%d\n", opHash->collide, opHash->count);
	printf("定石変換＆登録完了\n");
}