
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BitBoard.h"
#include "const.h"
#include "Container.h"
#include "CPU.h"

//ƒ{[ƒh¶¬
BitBoard *BitBoard_New(void) {
	BitBoard *res;
	res = (BitBoard*)malloc(sizeof(BitBoard));
	if (res) {
		BitBoard_Reset(res);
	}
	return res;
}

//ƒ{[ƒh‚Ìíœ
void BitBoard_Delete(BitBoard *bitboard) {
	free(bitboard);
}

//”Õ–Ê‚Ì‰Šú‰»
void BitBoard_Reset(BitBoard *bitboard) {
	bitboard->white = 0x8042201008040201;//^‚ñ’†“ñ‚ÂˆÈŠO0
	bitboard->black = 0x0000000810000000;//^‚ñ’†“ñ‚ÂˆÈŠO0
}

//”Õ–Ê‚Ì•`‰æ
void BitBoard_Draw(const BitBoard *bitboard) {
	char x, y;

	BitBoard cpyBoard = *bitboard;

	printf("\n\n");
	printf(" @@@@‚`  ‚a  ‚b  ‚c  ‚d  ‚e  ‚f  ‚g                        •:  vs  ”’:\n"/*, board->Black, board->White*/);
	printf(" @{\{\{\{\{\{\{\{\{\{\{\n");
	printf(" @b”b”b”b”b”b”b”b”b”b”b\n");
	printf(" @{\{\{\{\{\{\{\{\{\{\{\n");



	for (x = 0; x < BitBOARD_SIZE * BitBOARD_SIZE; x++, cpyBoard.white<<=1, cpyBoard.black<<=1) {
		if (x%8 == 0) {
			printf(" %d b”b", x/8+1);
		}
		if (cpyBoard.white & 0x8000000000000000) {
			printf("œb");
		}
		else if (cpyBoard.black & 0x8000000000000000) {
			printf("Zb");
		}
		else {
			printf("@b");
		}
		if (x % 8 == 7) {

			printf("”b %d", x / 8+1);
			putchar('\n');
			printf(" @{\{\{\{\{\{\{\{\{\{\{\n");
		}
	}
	printf(" @b”b”b”b”b”b”b”b”b”b”b\n");
	printf(" @{\{\{\{\{\{\{\{\{\{\{\n");
	printf(" @@@@‚`  ‚a  ‚b  ‚c  ‚d  ‚e  ‚f  ‚g\n");

}

//color‚ÌÎ”‚ğ•Ô‚·
char BitBoard_CountStone(const BitBoard *bitboard, char in_color) {
	return 0;
}

//’…è
char BitBoard_Flip(BitBoard *bitboard, char color, char pos) {

	char flipCount = 0;

	return flipCount;
}

//’¼ü‚ğ— •Ô‚·
char BitBoard_FlipLine(BitBoard *bitboard, char color, char pos, char vec) {
	char oppColor = getOppStone(color);
	char flipCount = 0;

	pos += vec;

	return flipCount;
}

//ˆêè–ß‚·
void BitBoard_Undo(BitBoard *bitboard) {
	char pos;
}

//’…è‚Å‚«‚é‚©
char BitBoard_CanPlay(BitBoard *bitboard, char color) {
	int i;

	return FALSE;
}

//pos‚É’…è‚Å‚«‚é‚©
char BitBoard_CanFlip(const BitBoard *bitboard, char color, char pos) {



	return FALSE;
}

//Board_CanFlip‚Ì•â•
char BitBoard_CanFlipLine(const BitBoard *bitboard, char color, char pos, char vec) {
	char oppColor = getOppStone(color);
	char count = 0;

	return count;
}

//”½“]”‚ğ•Ô‚·
char BitBoard_CountFlips(const BitBoard *bitboard, char color, char pos) {
	char flipCount = 0;


	return flipCount;
}

uint64 getBitPos(char x, char y) {
	return (uint64)1 << (x + y * BOARD_SIZE);
}



