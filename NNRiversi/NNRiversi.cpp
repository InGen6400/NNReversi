// NNRiversi.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "BitBoard.h"
#include "Player.h"
#include "const.h"
#include "CPU.h"
#include "Pattern.h"
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <intrin.h>
#pragma comment(lib, "winmm.lib")

const char NODEF_MODE = -1;
const char BATTLE = 0;
const char LEARN = 1;
const char TIME = 2;
const char PVP = 3;
const char DEBUG = 4;

char AVX2_FLAG;

void Game_PVP(char showMobility);

void Game_Battle(char showMobility);

void Game_Time(char showMobility);

void MODE_DEBUG();

int main()
{
	char tmp[50] = "";
	char mode = -1;
	char showMobility = FALSE;
	int x, y;

#pragma region CAN_USE_AVX2
	int CPUInfo[4];
	__cpuidex(CPUInfo, 7, 0);
	if (CPUInfo[1] & (1 << 5)) {
		printf("CPUInfo[1]:%d\nAVX2�����p�\�ł��B\n�������[�h�ŋN�����܂��B\n", CPUInfo[1]);
		AVX2_FLAG = TRUE;
	}
	else {
		printf("CPUInfo[1]:%d\nAVX2�͗��p'�s'�\�ł��B\n�ᑬ���[�h�ŋN�����܂��B\n", CPUInfo[1]);
		AVX2_FLAG = FALSE;
	}

	Pattern_setAVX(AVX2_FLAG);
#pragma endregion

	printf("�ݒ�\n");
	
	while (mode == NODEF_MODE) {
		printf("���[�h(B:battle or L:Learning or T:Time P:PVP):");
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
			printf("����\�ȏꏊ��\�����܂�\n");
		}
		else if (tmp[0] == '@' && tmp[1] == 'D') {
			mode = DEBUG;
		}
		else {
			printf("���̂悤�ȃ��[�h�͑����Ă���܂���\n");
		}
	}
	if (mode == PVP) {
		Game_PVP(showMobility);
	}else if (mode == BATTLE) {
		Game_Battle(showMobility);
	}
	else if (mode == LEARN) {
		/*
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
					//CPU�̃^�[��
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
					printf("���Ԃ����ΐ�:%d\n", flipCount);
					turn = getOppStone(turn);
				}
				else {
					printf("You Can't Put (%c, %c)\n", "ABCDEFGH"[x], "12345678"[y]);
				}
				Board_Draw(mainBoard);

			}
		}
		*/
	}
	else if (mode == TIME) {
		Game_Time(FALSE);
	}
	else if (mode == DEBUG) {
		MODE_DEBUG();
	}
	
    return 0;
}

void Game_PVP(char showMobility) {

	BitBoard *bitboard;
	bitboard = BitBoard_New();

	int turn = BLACK;
	int x, y;

	char endFlag = FALSE;
	char tmp[10];
	char left = 60;
	char passed = FALSE;

	char player2Turn = WHITE;
	uint64 put;

	system("cls");
	BitBoard_Draw(bitboard, showMobility);
	while (!endFlag) {
		if (BitBoard_getMobility(bitboard->stone[turn], (bitboard->stone[oppColor(turn)])) > 0) {
			passed = FALSE;
			if (turn == player2Turn) {
				//�v���C���[�̃^�[��
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
							printf("�߂��܂���\n");
							BitBoard_Draw(bitboard, showMobility);
						}
						else {
							printf("����ȏ�߂��܂���\n");
						}
						continue;
					}
					else if (Player_Input(tmp, &x, &y) == FALSE) {
						printf("�����ƁA�낤����~����Ƃ���ł����B\n�ςȕ����͓���Ȃ��ł���������\n");
						continue;
					}
					put = getBitPos(x, y);
					break;
				}
			}
			else {
				//�v���C���[�̃^�[��
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
							printf("�߂��܂���\n");
							BitBoard_Draw(bitboard, showMobility);
						}
						else {
							printf("����ȏ�߂��܂���\n");
						}
						continue;
					}
					else if (Player_Input(tmp, &x, &y) == FALSE) {
						printf("�����ƁA�낤����~����Ƃ���ł����B\n�ςȕ����͓���Ȃ��ł���������\n");
						continue;
					}
					put = getBitPos(x, y);
					break;
				}
			}

			system("cls");
			if (BitBoard_Flip(bitboard, turn, put) >= 1) {
				if (turn == player2Turn) {
					printf("CPU Put (%c, %c)\n", "HGFEDCBA"[x], "87654321"[y]);
					left--;
				}
				else {
					printf("You Put (%c, %c)\n", "HGFEDCBA"[x], "87654321"[y]);
					left--;
				}
				turn = oppColor(turn);
			}
			else {
				printf("Can't put (%d,%d)\n", x-8, y-8);
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

	//CPU�̐F�ݒ�
	while (cpuTurn == -2)
	{
		printf("CPU�̐F(B:black or W:white):");
		fgets(tmp, sizeof(tmp), stdin);
		if (tmp[0] == 'B' || tmp[0] == 'b') {
			cpuTurn = BLACK;
		}
		else if (tmp[0] == 'W' || tmp[0] == 'w') {
			cpuTurn = WHITE;
		}
		else {
			printf("�����ƁA�낤���o�O��Ƃ���ł����B���J�ɓ��͂��Ă���������Ƃ��ꂵ���ł��B\n W, B�̂ǂꂩ�ł���H\n");
		}
	}
	system("cls");
	BitBoard_Draw(bitboard, showMobility);
	while (!endFlag) {
		if (BitBoard_getMobility(bitboard->stone[turn], (bitboard->stone[oppColor(turn)])) > 0) {
			passed = FALSE;
			if (turn == cpuTurn) {
				//CPU�̃^�[��
				printf("CPU Thinking...", turn);
				cpu->node = 0;
				cpu->start = timeGetTime();
				CPU_PUT(cpu, &put, turn, left);
				cpu->end = timeGetTime();
				printf("\nNegaAlpha: time:%d node:%d\n", cpu->end - cpu->start, cpu->node);
				getXY(put, &x, &y);
			}
			else {
				//�v���C���[�̃^�[��
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
							printf("�߂��܂���\n");
							BitBoard_Draw(bitboard, showMobility);
						}
						else {
							printf("����ȏ�߂��܂���\n");
						}
						continue;
					}
					else if (Player_Input(tmp, &x, &y) == FALSE) {
						printf("�����ƁA�낤����~����Ƃ���ł����B\n�ςȕ����͓���Ȃ��ł���������\n");
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
	CPU_Delete(cpu);
}

void Game_Time(char showMobility) {

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
	CPU *cpu2 = CPU_Init(bitboard);
	cpu2->start = timeGetTime();
	cpu->start = timeGetTime();

	//CPU�̐F�ݒ�
	system("cls");
	BitBoard_Draw(bitboard, showMobility);
	while (!endFlag) {
		if (BitBoard_getMobility(bitboard->stone[turn], (bitboard->stone[oppColor(turn)])) > 0) {
			passed = FALSE;
			if (turn == cpuTurn) {
				//CPU�̃^�[��
				cpu->node = 0;
				CPU_PUT(cpu, &put, turn, left);
				getXY(put, &x, &y);
			}
			else {
				//CPU2�̃^�[��
				cpu->node = 0;
				CPU_PUT(cpu, &put, turn, left);
				getXY(put, &x, &y);
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
	cpu2->end = timeGetTime();
	cpu->end = timeGetTime();
	printf("Time:%.4f", ((cpu2->end - cpu2->start))/1000.0);
	BitBoard_Delete(bitboard);
	CPU_Delete(cpu);
	CPU_Delete(cpu2);
}

void MODE_DEBUG() {

	BitBoard *bitboard;

	int turn = BLACK;
	int x, y;

	char endFlag = FALSE;
	char tmp[10];
	char cpuTurn = BLACK;
	char left = 60;
	char passed = FALSE;

	uint64 put;

	bitboard = BitBoard_New();
	CPU *cpu = CPU_Init(bitboard);
	CPU *cpu2 = CPU_Init(bitboard);
	cpu2->start = timeGetTime();
	cpu->start = timeGetTime();
	/*
	//CPU�̐F�ݒ�
	system("cls");
	BitBoard_Draw(bitboard, FALSE);
	while (!endFlag) {
		if (BitBoard_getMobility(bitboard->stone[turn], (bitboard->stone[oppColor(turn)])) > 0) {
			passed = FALSE;
			if (turn == cpuTurn) {
				//CPU�̃^�[��
				cpu->node = 0;
				CPU_PUT(cpu, &put, turn, left);
				getXY(put, &x, &y);
			}
			else {
				//CPU2�̃^�[��
				cpu->node = 0;
				CPU_PUT(cpu, &put, turn, left);
				getXY(put, &x, &y);
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
	*/
	printf("%d\n", getEdgeIndexDR_U(bitboard));
	cpu2->end = timeGetTime();
	cpu->end = timeGetTime();
	printf("Time:%.4f", ((cpu2->end - cpu2->start)) / 1000.0);
	BitBoard_Delete(bitboard);
	CPU_Delete(cpu);
	CPU_Delete(cpu2);
}