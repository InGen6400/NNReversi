// NNRiversi.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Board.h"
#include "Player.h"
#include <stdlib.h>
#include <string.h>


int main()
{
	char tmp[50] = "";
	Board *mainBoard;
	char turn = BLACK;
	int x, y;

	mainBoard = Board_New();
	Board_Draw(mainBoard);
	while (1) {
		fgets(tmp, sizeof(tmp), stdin);
		if (tmp[0] == 'q')break;
		if (tmp[0] == 'b') {
			if (*(mainBoard->Sp-1) != -2) {
				Board_Undo(mainBoard);
				turn = getOppStone(turn);
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
							printf("白 Put (%d, %d)", x, y);
						}
						else {
							printf("黒 Put (%d, %d)", x, y);
						}
						turn = getOppStone(turn);
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

