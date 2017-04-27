#include "opening.h"
#include "bsTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void open_read(BitBoard *board, OPNode *root) {
	FILE *fp;
	char buf[OPFILE_LINE_SIZE];
	OPdata *opdata;
	int value, min;
	int history_move[BITBOARD_SIZE * BITBOARD_SIZE * 2];
	char color, turn;
	int i;

	fp = fopen(OPEN_FILE_NAME, "r");
	if (!fp) {
		printf("Can't open opening file\n");
		return;
	}
	//ファイル読み込みが終わったらbreak
	while (1) {
		BitBoard_Reset(board);
		color = BLACK;
		turn = 0;
		if (fgets(buf, sizeof(buf), fp) == NULL) {
			printf("finish Read opening Book\n");
			break;
		}
		strtok(buf, " ");
		value = (int)(atof(strtok(NULL, "\n")) * STONE_VALUE);
	}
	fclose(fp);
}

