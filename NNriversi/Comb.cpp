#include <stdio.h>
#include <stdlib.h>
#include "stdafx.h"
#include "Comb.h"
#include "BitBoard.h"

void Comb_Init() {
	int i, j, turn, c, in, out;
	int corner_c[] = {POW3_2, POW3_5, POW3_0, POW3_3, POW3_6, POW3_1, POW3_4, POW3_7};
	for (turn = 0; turn < 16; turn++) {
		for (i = 0; i < COMB_AMOUNT; i++) {
			Value[turn][i] = (int*)calloc(CombIndex[i], sizeof(int));
			if (!Value[turn][i]) {
				printf("Comb-Comb_Init() Valueのメモリ確保に失敗しました\n");
				exit(EXIT_FAILURE);
			}
		}
	}

	for (i = 0; i < POW3_8; i++) {
		in = i;
		out = 0;
		c = POW3_7;
		for (j = 0; j < 8; j++) {
			out += in % 3 * c;
			in /= 3;
			c /= 3;
		}
		if (out < i) {
			MirrorLine_C[i] = out;
		}
		else {
			MirrorLine_C[i] = i;
		}
	}

	for (i = 0; i < POW3_8; i++) {
		in = i;
		out = 0;
		for (j = 0; j < 8; j++) {
			out += in % 3 * corner_c[j];
			in /= 3;
		}
		if (out < i) {
			MirrorCorner_C[i] = out;
		}
		else {
			MirrorCorner_C[i] = i;
		}
	}
}

void Comb_Delete() {
	for (int i = 0; i < COMB_AMOUNT; i++) {
		if (Value[i]) {
			free(Value[i]);
		}
	}
}

int Comb_Load() {
	FILE *fp;
	int i;

	fp = fopen(COMBFILE, "rb");
	if (!fp) {
		printf("ファイルが開けませんでした\n");
		return 0;
	}
	for (i = 0; i < COMB_AMOUNT; i++) {
		if (fread(Value[i], sizeof(int), CombIndex[i], fp) < (size_t)CombIndex[i]) {
			fclose(fp);
			return 0;
		}
	}
	return 1;
}

int Comb_Save() {
	FILE *fp;
	int i;

	fp = fopen(COMBFILE, "wb");
	if (!fp) {
		printf("ファイルが開けませんでした\n");
		return 0;
	}
	if (fwrite(Value[i], sizeof(int), CombIndex[i], fp) < (size_t)CombIndex[i]) {
		fclose(fp);
		printf("COMB読み込み失敗\n");
		return 0;
	}
	fclose(fp);
	return 1;
}

void Comb_Debug(int index) {
	printf("%d\n", index);
	printf("%d\n", MirrorLine_C[index*POW3_2]);
}