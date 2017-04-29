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