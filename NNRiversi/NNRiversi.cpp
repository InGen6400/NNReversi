// NNRiversi.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Board.h"
#include "Player.h"
#include "const.h"
#include "CPU.h"
#include <stdlib.h>
#include <string.h>


int main()
{
	char tmp[50] = "";
	Board *mainBoard;
	char cpuPut = 0, turn = BLACK;
	int x, y;

	mainBoard = Board_New();
	Board_Draw(mainBoard);
	while (1) {
		fgets(tmp, sizeof(tmp), stdin);
		if (tmp[0] == 'q')break;
		if (tmp[0] == 'b') {
			if (*(mainBoard->Sp-1) != -2) {
				Board_Undo(mainBoard);
				Board_Undo(mainBoard);
				system("cls");
				printf("戻しました\n");
				Board_Draw(mainBoard);
			}
			else {
				printf("これ以上戻せません\n");
			}
		}
		else 
		{
			if (Player_Input(tmp, &x, &y)) {
				if (x >= 1 && y >= 1 && x <= BOARD_SIZE && y <= BOARD_SIZE) {
					system("cls");
					if (Board_Flip(mainBoard, turn, x, y) >= 1) {
						if (turn == WHITE) {
							printf("白 Put (%d, %d)\n", x, y);
						}
						else {
							printf("黒 Put (%d, %d)\n", x, y);
						}
						turn = getOppStone(turn);
						NegaMaxSearch(mainBoard, FALSE, turn, 0, &cpuPut);
						if (turn == WHITE) {
							x = getX(cpuPut);
							y = getY(cpuPut);
							printf("CPU : %d,%d(%d)\n", x, y, cpuPut);
							printf("flip:%d\n", Board_Flip(mainBoard, turn, x, y));
							turn = getOppStone(turn);
						}
					}
					else {
						printf("You Can't Put (%d, %d)\n", x, y);
					}
					Board_Draw(mainBoard);
				}
				else {
					printf("You Can't Place that position\n");
				}
			}
			else {
				printf("おっと、危うく停止するところでした。\n変な文字は入れないでくださいね\n");
			}
		}
	}
	Board_Delete(mainBoard);
    return 0;
}

