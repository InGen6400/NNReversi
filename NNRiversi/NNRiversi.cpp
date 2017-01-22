// NNRiversi.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Board.h"
#include "Player.h"
#include "const.h"
#include "CPU.h"
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#pragma comment(lib, "winmm.lib")

int main()
{
	char tmp[50] = "";
	Board *mainBoard;
	CPU *cpu;
	CPU *cpu2;
	char cpuTurn = NOMOVE, cpuPut = 0, turn = BLACK, flipCount = 0;
	char endFlag = FALSE;
	char passed = FALSE;
	char left = 64;
	int x, y;

	printf("設定\n");
	//CPUの色設定
	while (cpuTurn == -2)
	{
		printf("CPUの色(B:black or W:white or N:NoCpu):");
		fgets(tmp, sizeof(tmp), stdin);
		if (tmp[0] == 'B' || tmp[0] == 'b') {
			cpuTurn = BLACK;
		}
		else if (tmp[0] == 'W' || tmp[0] == 'w') {
			cpuTurn = WHITE;
		}
		else if (tmp[0] == 'N' || tmp[0] == 'n') {
			cpuTurn = -1;
		}
		else {
			printf("おっと、危うくバグるところでした。丁寧に入力していただけるとうれしいです。\n W, B, Nのどれかですよ？\n");
		}
	}

	system("cls");
	mainBoard = Board_New();
	cpu = CPU_Init(mainBoard);
	cpu2 = CPU_Init(mainBoard);
	left = 60;
	Board_Draw(mainBoard);
	while (!endFlag) {
		if (turn == cpuTurn) {
			if (Board_CanPlay(mainBoard, turn) == TRUE) {
				passed = false;
				//CPUのターン
				printf("CPU Thinking...");
				cpu->node = 0;
				cpu->start = timeGetTime();
				CPU_PUT(cpu, &cpuPut, turn, left);
				cpu->end = timeGetTime();
				x = getX(cpuPut);
				y = getY(cpuPut);
				printf("\nNegaAlpha: time:%d node:%d\n", cpu->end - cpu->start, cpu->node);
				printf("CPU Put (%c, %c)\n", "ABCDEFGH"[x - 1], "12345678"[y - 1]);
				/*
				cpu->start = timeGetTime();
				cpu->node = 0;
				cpu->start = timeGetTime();
				NegaMaxSearch(cpu, FALSE, turn, 0, &cpuPut);
				cpu->end = timeGetTime();
				x = getX(cpuPut);
				y = getY(cpuPut);
				printf("NegaMax: time:%d node:%d\n", cpu->end - cpu->start, cpu->node);
				printf("CPU Put (%c, %c)\n", "ABCDEFGH"[x - 1], "12345678"[y - 1]);*/
				//scanf("%c", &tmp);
			}
			else {
				x = -1;
				if (passed) {
					endFlag = true;
				}
				else {
					passed = true;
				}
			}
		}
		else {
			//プレイヤーのターン
				/*
			while (1) {
				fgets(tmp, sizeof(tmp), stdin);
				if (tmp[0] == 'q') {
					endFlag = TRUE;
					break;
				}
				else if (tmp[0] == '.' && tmp[1] == '.') {
					if (*(mainBoard->Sp - 1) != -2) {
						Board_Undo(mainBoard);
						Board_Undo(mainBoard);
						system("cls");
						printf("戻しました\n");
						Board_Draw(mainBoard);
					}
					else {
						printf("これ以上戻せません\n");
					}
					continue;
				}
				else if (Player_Input(tmp, &x, &y) == FALSE) {
					printf("おっと、危うく停止するところでした。\n変な文字は入れないでくださいね\n");
					continue;
				}
				break;
			}*/
			if (Board_CanPlay(mainBoard, turn) == TRUE) {
				passed = false;
				printf("CPU2 Thinking...");
				cpu2->node = 0;
				cpu2->start = timeGetTime();
				CPU_PUT(cpu2, &cpuPut, turn, left);
				cpu2->end = timeGetTime();
				x = getX(cpuPut);
				y = getY(cpuPut);
			}
			else {
				x = -1;
				if (passed) {
					endFlag = true;
				}
				else {
					passed = true;
				}
			}
		}
		if (x == -1) {
			turn = getOppStone(turn);
			if (turn == cpuTurn) {
				printf("CPU pass\n");
			}
			else {
				printf("You pass\n");
			}

		}else if (x >= 1 && y >= 1 && x <= BOARD_SIZE && y <= BOARD_SIZE) {
			system("cls");
			if (flipCount = Board_Flip(mainBoard, turn, ConvertPos(x,y)) >= 1) {
				if (turn == cpuTurn) {
					printf("CPU Put (%c, %c)\n", "ABCDEFGH"[x - 1], "12345678"[y - 1]);
					left--;
				}
				else {
					printf("You Put (%c, %c)\n", "ABCDEFGH"[x - 1], "12345678"[y - 1]);
					left--;
				}
				printf("裏返した石数:%d\n", flipCount);
				turn = getOppStone(turn);
			}
			else {
				printf("You Can't Put (%c, %c)\n", "ABCDEFGH"[x - 1], "12345678"[y - 1]);
			}
			Board_Draw(mainBoard);

		}
	}
	Board_Delete(mainBoard);
    return 0;
}

