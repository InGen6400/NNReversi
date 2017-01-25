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

	}
	else if (mode == LEARN) {
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

			}
			else if (x >= 1 && y >= 1 && x <= BOARD_SIZE && y <= BOARD_SIZE) {
				system("cls");
				if (flipCount = Board_Flip(mainBoard, turn, ConvertPos(x, y)) >= 1) {
					if (turn == cpuTurn) {
						printf("CPU Put (%c, %c)\n", "ABCDEFGH"[x ], "12345678"[y]);
						left--;
					}
					else {
						printf("You Put (%c, %c)\n", "ABCDEFGH"[x ], "12345678"[y]);
						left--;
					}
					printf("裏返した石数:%d\n", flipCount);
					turn = getOppStone(turn);
				}
				else {
					printf("You Can't Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
				}
				Board_Draw(mainBoard);

			}
		}
	}
	else if (mode == TIME) {
		system("cls");
		cpuTurn = BLACK;
		mainBoard = Board_New();
		cpu = CPU_Init(mainBoard);
		cpu2 = CPU_Init(mainBoard);
		cpu->start = timeGetTime();
		left = 60;
		Board_Draw(mainBoard);
		while (!endFlag) {
			if (turn == cpuTurn) {
				if (Board_CanPlay(mainBoard, turn) == TRUE) {
					passed = false;
					//CPUのターン
					printf("CPU Thinking...");
					cpu->node = 0;
					CPU_PUT(cpu, &cpuPut, turn, left);
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
			else {
				if (Board_CanPlay(mainBoard, turn) == TRUE) {
					passed = false;
					printf("CPU2 Thinking...");
					cpu2->node = 0;
					CPU_PUT(cpu2, &cpuPut, turn, left);
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

			}
			else if (x >= 1 && y >= 1 && x <= BOARD_SIZE && y <= BOARD_SIZE) {
				system("cls");
				if (flipCount = Board_Flip(mainBoard, turn, ConvertPos(x, y)) >= 1) {
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
				Board_Draw(mainBoard);

			}
		}
		cpu->end = timeGetTime();
		printf("\nNegaAlpha: time:%d\n", cpu->end - cpu->start);
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
			if (BitBoard_getMobility(bitboard, turn) > 0) {
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
}

/*
void Game_Battle() {
	BitBoard *bitboard;
	char endFlag = FALSE;
	int turn = BLACK;
	int x, y;
	char tmp[10];

	char cpuTurn = BLACK;
	CPU *cpu = CPU_Init(mainBoard);
	cpu->start = timeGetTime();
	char left = 60;

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
				x = getX(cpuPut) - 1;
				y = getY(cpuPut) - 1;
				printf("\nNegaAlpha: time:%d node:%d\n", cpu->end - cpu->start, cpu->node);
				printf("CPU Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
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

		}
		else if (x >= 1 && y >= 1 && x <= BOARD_SIZE && y <= BOARD_SIZE) {
			system("cls");
			if (flipCount = Board_Flip(mainBoard, turn, ConvertPos(x + 1, y + 1)) >= 1) {
				if (turn == cpuTurn) {
					printf("CPU Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
					left--;
				}
				else {
					printf("You Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
					left--;
				}
				printf("裏返した石数:%d\n", flipCount);
				turn = oppColor(turn);
			}
			else {
				printf("You Can't Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
			}
			Board_Draw(mainBoard);

		}
	}
}
*/