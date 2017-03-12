// NNRiversi.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include <stdio.h>
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

enum {
	NODEF_MODE = -1,
	BATTLE,
	TIME,
	PVP,
	LEARN,
	READ,
	DEBUG
};

const short LINE_MAX = 200;

char AVX2_FLAG;

void Game_PVP(char showMobility);

void Game_Battle(char showMobility);

void Game_Time(char showMobility);

void MODE_DEBUG();

void MODE_READBOOK();

void MODE_LEARN();

int main()
{
	char tmp[50] = "";
	char mode = -1;
	char showMobility = FALSE;
	int x, y;

//AVX2�����p�\���ǂ����̔���
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
		else if (tmp[0] == 'R' || tmp[0] == 'r') {
			mode = READ;
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
		MODE_LEARN();
	}
	else if (mode == TIME) {
		Game_Time(FALSE);
	}
	else if (mode == DEBUG) {
		MODE_DEBUG();
	}
	else if (mode == READ) {
		MODE_READBOOK();
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
							turn = BitBoard_Undo(bitboard);
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
							turn = BitBoard_Undo(bitboard);
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

	CPU *cpu = CPU_Init();
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
				system("cls");
				//CPU�̃^�[��
				printf("CPU Thinking...");
				cpu->leaf = 0;
				cpu->start = timeGetTime();
				CPU_Move(cpu, bitboard, &put, turn, left);
				cpu->end = timeGetTime();
				printf("\nNegaAlpha: time:%d node:%d\n", cpu->end - cpu->start, cpu->leaf);
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
							//�O���CPU�̃^�[���܂Ŗ߂�
							do {
								turn = BitBoard_Undo(bitboard);
							} while (turn == cpuTurn);
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

			//system("cls");
			if (BitBoard_Flip(bitboard, turn, put) >= 1) {
				if (turn == cpuTurn) {
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
				printf("Can't put (%d,%d)\n", x - 8, y - 8);
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

	CPU *cpu = CPU_Init();
	CPU *cpu2 = CPU_Init();
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
				cpu->leaf = 0;
				CPU_Move(cpu, bitboard, &put, turn, left);
				getXY(put, &x, &y);
			}
			else {
				//CPU2�̃^�[��
				cpu->leaf = 0;
				CPU_Move(cpu, bitboard, &put, turn, left);
				getXY(put, &x, &y);
			}

			system("cls");
			if (BitBoard_Flip(bitboard, turn, put) >= 1) {
				if (turn == cpuTurn) {
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
				printf("Can't put (%d,%d)\n", x - 8, y - 8);
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
	CPU *cpu = CPU_Init();
	CPU *cpu2 = CPU_Init();
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
			else {
				printf("Can't put (%d,%d)\n", x-8, y-8);
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
	
	put=getPos_book_upper("D3");

	cpu2->end = timeGetTime();
	cpu->end = timeGetTime();
	printf("Time:%.4f", ((cpu2->end - cpu2->start)) / 1000.0);
	BitBoard_Delete(bitboard);
	CPU_Delete(cpu);
	CPU_Delete(cpu2);
}

void MODE_LEARN() {

}

void MODE_READBOOK() {
	//�t�@�C���p�X�Ɠǂݍ��݌�̃t�@�C���p�X
	char path[1000], loadedPath[1000];
	//��s���̕�����
	char Line[LINE_MAX];
	//����ʒu�����̃f�[�^
	char *positions;
	//����f�[�^�̈ꎟ�L���ϐ�, ���ݎ�Ԃ̐F
	char c_pos[5], color=BLACK;
	//�C�e���[�^, �^�[����, �΍�, �ǂݍ��񂾍s��
	int i, left=60, diff, dataCount = 0, randomTurn;
	FILE *fp;
	BitBoard *bitboard = BitBoard_New();
	//����ʒu
	uint64 put;
	Pattern_Init();

	Pattern_Load();

	printf("BOOK Path:");
	fgets(path, sizeof(path), stdin);
	strtok(path, "\n");
	fp = fopen(path, "r");

	if (!fp) {
		printf("can't open file : %s", path);
		return;
	}

	while (fgets(Line, sizeof(Line), fp)) {
		BitBoard_Reset(bitboard);
		left = 60;
		color = BLACK;

		//correctbk�͒ʂ��ԍ�������̂ŃJ�b�g
		strtok(Line, " ");
		//���Ղ̃����_���͖������^�[���J�E���g�����i�߂�
		positions = strtok(NULL, " ");
		for (i = 0; positions[i] != '\0'; i += 2) {
			c_pos[0] = positions[i];
			c_pos[1] = positions[i + 1];
			put = getPos_book_lower(c_pos);
			//��������ł��Ȃ������ꍇ�p�X�Ƃ��ĐF��ς��Ē���
			if (BitBoard_Flip(bitboard, color, put) == 0) {
				color = oppColor(color);
				if (BitBoard_Flip(bitboard, color, put) == 0) {
					printf("iligal data >> skip read RANDOM\n");
				}
			}
			left--;
			//�p�^�[���̍X�V�͂��Ȃ�
			color = oppColor(color);
		}
		randomTurn = left;
		//�󔒈ȍ~�̊������擾
		positions = strtok(NULL, " ");
		printf("positions : %s\n", positions);
		//�΍��擾(�����_)
		diff = atoi(strtok(NULL, " "));

		for (i = 0; positions[i] != NULL; i+=2) {
			//�񕶎��������o��(A1 etc...)
			c_pos[0] = positions[i];
			c_pos[1] = positions[i + 1];
			put = getPos_book_lower(c_pos);
			printf("dataCount:%d randomTurn:%d left:%d\n", dataCount, randomTurn, left);
			//��������ł��Ȃ������ꍇ�p�X�Ƃ��ĐF��ς��Ē���
			if (BitBoard_Flip(bitboard, color, put) == 0) {
				color = oppColor(color);
				if (BitBoard_Flip(bitboard, color, put) == 0) {
					printf("iligal data >> skip read\n");
				}
			}
			//�p�^�[���̕]���l�̍X�V
			UpdateAllPattern(bitboard, diff, left);
			color = oppColor(color);
			left--;
		}
		dataCount++;
		//1000�Ɉ�x�\��
		if (dataCount % 1000 == 0) {
			printf("Finish Reading : %d", dataCount);
		}
	}

	fclose(fp);

	Pattern_Save();

	//fgets�̉��s�𖳎�
	strtok(path, "\n");

	//�t�@�C������ύX
	sprintf(loadedPath, "%s.end", path);
	if (rename(path, loadedPath) == 0) {
		printf("�t�@�C�����ύX:%s->%s\n", path, loadedPath);
	}
}