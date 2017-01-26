// NNRiversi.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Board.h"
#include "BitBoard.h"
#include "Player.h"
#include "const.h"
#include "CPU.h"
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#pragma comment(lib, "winmm.lib")

const char NODEF_MODE = -1;
const char BATTLE = 0;
const char LEARN = 1;
const char TIME = 2;
const char PVP = 3;

void Game_PVP(char showMobility);
void Game_Battle(char showMobility);
void Game_Learn();
void Game_Time();

int main()
{
	char tmp[50] = "";
	Board *mainBoard;
	CPU *cpu;
	CPU *cpu2;
	char cpuTurn = NOMOVE, cpuPut = 0, turn = BLACK, flipCount = 0;
	char endFlag = FALSE;
	char passed = FALSE;
	char mode = -1;
	char left = 64;
	char showMobility = FALSE;
	int x, y;

	printf("設定\n");
	
	while (mode == NODEF_MODE) {
		printf("モード(B:battle or L:Learning or T:Time P:PVP):");
		fgets(tmp, sizeof(tmp), stdin);
		if (tmp[0] == 'B' || tmp[0] == 'b') {
			mode = BATTLE;
		}
		else if (tmp[0] == 'L' || tmp[0] == 'l') {
			mode = LEARN;
		}
		else if (tmp[0] == 'T' || tmp[0] == 't') {
			mode = TIME;
		}
		else if (tmp[0] == 'P' || tmp[0] == 'p') {
			mode = PVP;
		}
		else if (tmp[0] == '.') {
			showMobility = TRUE;
			printf("着手可能な場所を表示します\n");
		}
		else {
			printf("そのようなモードは存じておりません\n");
		}
	}
	/*
	bitboard = BitBoard_New();
	fgets(tmp, sizeof(tmp), stdin);
	if (Player_Input(tmp, &x, &y) == FALSE) {
		printf("おっと、危うく停止するところでした。\n変な文字は入れないでくださいね\n");
	}
	else {
		printf("x:%d, y:%d\n", x, y);
		BitBoard_Flip(bitboard, WHITE, getBitPos(x, y));
	}
	BitBoard_Draw(bitboard);
	*/
	if (mode == PVP) {
		Game_PVP(showMobility);
	}else if (mode == BATTLE) {
		Game_Battle(showMobility);
	}
	else if (mode == LEARN) {
		Game_Learn();
	}
	else if (mode == TIME) {
		Game_Time();
	}
	
	//Board_Delete(mainBoard);
	//BitBoard_Delete(bitboard);
    return 0;
}

void Game_PVP(char showMobility) {
	BitBoard *bitboard;
	char endFlag = FALSE;
	int turn = BLACK;
	int x, y;
	char tmp[10];

	system("cls");
	bitboard = BitBoard_New();
	BitBoard_Draw(bitboard, showMobility);
	while (!endFlag) {
		while (1) {
			//置けない場合はパスになる(未確認)
			if (BitBoard_getMobility(bitboard->stone[turn], turn) > 0) {
				fgets(tmp, sizeof(tmp), stdin);
				if (tmp[0] == 'q') {
					endFlag = TRUE;
					break;
				}
				else if (tmp[0] == '.' && tmp[1] == '.') {
					if (*(bitboard->Sp - 1) != -2) {
						BitBoard_Undo(bitboard);
						BitBoard_Undo(bitboard);
						system("cls");
						printf("戻しました\n");
						BitBoard_Draw(bitboard, showMobility);
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
			}
			else {
				printf("Player passed");
				turn = oppColor(turn);
			}
			break;
		}

		if (x >= 0 && y >= 0 && x <= BITBOARD_SIZE && y <= BITBOARD_SIZE) {
			system("cls");
			if (BitBoard_Flip(bitboard, turn, getBitPos(x, y)) >= 1) {
				if (turn == BLACK) {
					printf("BLACK Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
				}
				else {
					printf("WHITE Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
				}
				turn = oppColor(turn);
			}
			BitBoard_Draw(bitboard, showMobility);

		}
	}
	BitBoard_Delete(bitboard);
}


void Game_Battle(char showMobility) {

	BitBoard *bitboard;
	bitboard = BitBoard_New();

	int turn = BLACK;
	int x, y;

	char endFlag = FALSE;
	char tmp[10];
	char cpuTurn = -2;
	char left = 60;
	char passed = FALSE;

	uint64 put;

	CPU *cpu = CPU_Init(bitboard);
	cpu->start = timeGetTime();

	//CPUの色設定
	while (cpuTurn == -2)
	{
		printf("CPUの色(B:black or W:white):");
		fgets(tmp, sizeof(tmp), stdin);
		if (tmp[0] == 'B' || tmp[0] == 'b') {
			cpuTurn = BLACK;
		}
		else if (tmp[0] == 'W' || tmp[0] == 'w') {
			cpuTurn = WHITE;
		}
		else {
			printf("おっと、危うくバグるところでした。丁寧に入力していただけるとうれしいです。\n W, Bのどれかですよ？\n");
		}
	}
	system("cls");
	BitBoard_Draw(bitboard, showMobility);
	while (!endFlag) {
		if (BitBoard_getMobility(bitboard->stone[turn], (bitboard->stone[oppColor(turn)])) > 0) {
			passed = FALSE;
			if (turn == cpuTurn) {
				//CPUのターン
				printf("CPU Thinking...", turn);
				cpu->node = 0;
				cpu->start = timeGetTime();
				CPU_PUT(cpu, &put, turn, left);
				cpu->end = timeGetTime();
				printf("\nNegaAlpha: time:%d node:%d\n", cpu->end - cpu->start, cpu->node);
				getXY(put, &x, &y);
			}
			else {
				//プレイヤーのターン
				while (1) {
					fgets(tmp, sizeof(tmp), stdin);
					if (tmp[0] == 'q') {
						endFlag = TRUE;
						break;
					}
					else if (tmp[0] == '.' && tmp[1] == '.') {
						if (*(bitboard->Sp - 1) != -2) {
							BitBoard_Undo(bitboard);
							BitBoard_Undo(bitboard);
							system("cls");
							printf("戻しました\n");
							BitBoard_Draw(bitboard, showMobility);
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
					put = getBitPos(x, y);
					break;
				}
			}

			system("cls");
			if (BitBoard_Flip(bitboard, turn, put) >= 1) {
				if (turn == cpuTurn) {
					printf("CPU Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
					left--;
				}
				else {
					printf("You Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
					left--;
				}
				turn = oppColor(turn);
			}

			BitBoard_Draw(bitboard, showMobility);


		}
		else {
			if (passed) {
				endFlag = TRUE;
			}
			else {
				passed = TRUE;
				turn = oppColor(turn);
			}
		}
	}
	BitBoard_Delete(bitboard);
}

void Game_Learn() {
	
	BitBoard *bitboard;
	bitboard = BitBoard_New();

	int turn = BLACK;
	int x, y;

	char endFlag = FALSE;
	char tmp[10];
	char cpuTurn = BLACK;
	char left = 60;
	char passed = FALSE;

	uint64 put;

	CPU *cpu = CPU_Init(bitboard);
	cpu->start = timeGetTime();

	system("cls");
	BitBoard_Draw(bitboard, FALSE);
	while (!endFlag) {
		if (BitBoard_getMobility(bitboard->stone[turn], (bitboard->stone[oppColor(turn)])) > 0) {
			passed = FALSE;

			printf("CPU Thinking...", turn);
			cpu->node = 0;
			CPU_PUT(cpu, &put, turn, left);
			printf("\nNegaAlpha: time:%d node:%d\n", cpu->end - cpu->start, cpu->node);
			getXY(put, &x, &y);

			system("cls");
			if (BitBoard_Flip(bitboard, turn, put) >= 1) {
				if (turn == cpuTurn) {
					printf("CPU Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
					left--;
				}
				else {
					printf("You Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
					left--;
				}
				turn = oppColor(turn);
			}

			BitBoard_Draw(bitboard, FALSE);


		}
		else {
			if (passed) {
				endFlag = TRUE;
			}
			else {
				passed = TRUE;
				turn = oppColor(turn);
			}
		}
	}
	cpu->end = timeGetTime();
	printf("Time : %d", cpu->end - cpu->start);
	BitBoard_Delete(bitboard);
}

void Game_Time() {
	BitBoard *bitboard;
	bitboard = BitBoard_New();

	int turn = BLACK;
	int x, y;

	char endFlag = FALSE;
	char tmp[10];
	char cpuTurn = BLACK;
	char left = 60;
	char passed = FALSE;

	uint64 put;

	system("cls");
	CPU *cpu = CPU_Init(bitboard);
	CPU *cpu2 = CPU_Init(bitboard);
	cpu->start = timeGetTime();
	left = 60;
	BitBoard_Draw(bitboard, FALSE);
	while (!endFlag) {
		if (turn == cpuTurn) {
			if (BitBoard_getMobility(bitboard->stone[turn], (bitboard->stone[oppColor(turn)])) > 0) {
				passed = false;
				//CPUのターン

				printf("CPU Thinking...", turn);
				cpu->node = 0;
				CPU_PUT(cpu, &put, turn, left);
				printf("\nNegaAlpha: time:%d node:%d\n", cpu->end - cpu->start, cpu->node);
				getXY(put, &x, &y);
			}
			else {
				if (passed) {
					endFlag = TRUE;
				}
				else {
					passed = TRUE;
					turn = oppColor(turn);
				}
			}
		}
		else {
			if (BitBoard_getMobility(bitboard->stone[turn], (bitboard->stone[oppColor(turn)])) > 0) {
				passed = false;
				printf("CPU2 Thinking...", turn);
				cpu->node = 0;
				CPU_PUT(cpu, &put, turn, left);
				printf("\nNegaAlpha: time:%d node:%d\n", cpu->end - cpu->start, cpu->node);
				getXY(put, &x, &y);
			}
			else {
				if (passed) {
					endFlag = TRUE;
				}
				else {
					passed = TRUE;
					turn = oppColor(turn);
				}
			}
		}
		system("cls");
		if (BitBoard_Flip(bitboard, turn, put) >= 1) {
			if (turn == cpuTurn) {
				printf("CPU Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
				left--;
			}
			else {
				printf("You Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
				left--;
			}
			turn = oppColor(turn);
		}
		BitBoard_Draw(bitboard, FALSE);

	}
	cpu->end = timeGetTime();
	printf("\nNegaAlpha: time:%d\n", cpu->end - cpu->start);
	BitBoard_Delete(bitboard);
}