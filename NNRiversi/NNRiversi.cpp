// NNRiversi.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include <stdio.h>
#include "BitBoard.h"
#include "Player.h"
#include "const.h"
#include "CPU.h"
#include "Pattern.h"
#include "Flags.h"
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <time.h>
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

#ifdef __AVX2__
	printf("compiled AVX2!!!\n");
#elif __AVX__
	printf("compiled AVX!!!\n");
#endif


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
	putchar('\n');
	Pattern_Init();
	//Pattern_Save();
	Pattern_Load();

	Board_InitConst();

	printf("\n�ݒ�\n");


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
						if (*(bitboard->Sp - 1) != 0xFFFFFFFFFFFFFFFFULL) {
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
						if (*(bitboard->Sp - 1) != 0xFFFFFFFFFFFFFFFFULL) {
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
				printf("Can't put (%c,%c)\n", "HGFEDCBA"[x], "87654321"[y]);
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

	int color = BLACK;
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
		if ((bitboard->stone[BLACK] & bitboard->stone[WHITE]) != 0) {
			
			drawBits(bitboard->stone[BLACK] & bitboard->stone[WHITE]);
		}
		if (BitBoard_getMobility(bitboard->stone[color], (bitboard->stone[oppColor(color)])) > 0) {
			passed = FALSE;
			if (color == cpuTurn) {
				//CPU�̃^�[��
				printf("CPU Thinking...\n");
				cpu->leaf = 0;
				cpu->start = timeGetTime();
				printf("value:%d\n", CPU_Move(cpu, bitboard, &put, color, left));
				cpu->end = timeGetTime();
				getXY(put, &x, &y);
			}
			else {
				//�v���C���[�̃^�[��
				while (1) {
					printf("���Ȃ��̔Ԃł�:");
					fgets(tmp, sizeof(tmp), stdin);
					if (tmp[0] == 'q') {
						endFlag = TRUE;
						break;
					}
					else if (tmp[0] == '.' && tmp[1] == '.') {
						if (*(bitboard->Sp - 1) != 0xFFFFFFFFFFFFFFFFULL) {
							//�O���CPU�̃^�[���܂Ŗ߂�
							do {
								if (*(bitboard->Sp - 1) != 0xFFFFFFFFFFFFFFFFULL) {
									break;
								}
								color = BitBoard_Undo(bitboard);
							} while (color == cpuTurn);
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
			if ((BitBoard_getMobility(bitboard->stone[color], bitboard->stone[oppColor(color)]) & put) != 0) {
				if (BitBoard_Flip(bitboard, color, put) >= 1) {
					//system("cls");
					BitBoard_Draw(bitboard, showMobility);
					if (color == cpuTurn) {
						printf("CPU Put (%c, %c)\n", "HGFEDCBA"[x], "87654321"[y]);
						printf("NegaAlpha: time:%d node:%d\n\n", cpu->end - cpu->start, cpu->leaf);
						left--;
					}
					else {
						printf("You Put (%c, %c)\n", "HGFEDCBA"[x], "87654321"[y]);
						left--;
					}
					color = oppColor(color);
				}
				else {
					printf("Can't put (%c,%c)\n", "HGFEDCBA"[x], "87654321"[y]);
				}
			}
			else {
				printf("Can't put stone (%c,%c)\n", "HGFEDCBA"[x], "87654321"[y]);
			}

		}
		else {
			if (passed) {
				endFlag = TRUE;
			}
			else {
				passed = TRUE;
				color = oppColor(color);
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

inline int get_rand(int max)
{
	return (int)((double)max * rand() / (RAND_MAX + 1.0));
}


void MODE_LEARN() {
	BitBoard *bitboard = BitBoard_New();
	CPU *cpu = CPU_Init();
	uint64 move;
	int history[BOARD_SIZE * BOARD_SIZE];
	int i, j, num, turn, value;
	int left;
	char color;
	int result;
	srand((unsigned)time(NULL));
	printf("�ΐ��:");
	scanf("%d", &num);

	CPU_SetLevel(cpu, 0, 4, 12);
	getchar();
	for (i = 0; i < num; i++) {
		BitBoard_Reset(bitboard);
		color = BLACK;
		left = 60;
		printf("skip op\n");
		for (j = 0; j < 8; j++) {
			if (BitBoard_getMobility(bitboard->stone[color], bitboard->stone[oppColor(color)]) > 0) {
				move_random(bitboard, color);
				history[left] = color;
				left--;
			}
			color = oppColor(color);
			/*system("cls");
			BitBoard_Draw(bitboard, FALSE);*/
			//getchar();
		}
		printf("read mid\n");
		while (1) {
			if (BitBoard_getMobility(bitboard->stone[color], bitboard->stone[oppColor(color)]) > 0) {
				if (left > 12 && get_rand(100) < 1) {
					move_random(bitboard, color);
				}
				else {
					value = CPU_Move(cpu, bitboard, &move, color, left);
					if (BitBoard_Flip(bitboard, color, move) == 0) {
						printf("white\n");
						drawBits(bitboard->stone[WHITE]);
						printf("black\n");
						drawBits(bitboard->stone[BLACK]);
						printf("can't put   color:%d\n", color);
						drawBits(move);
						printf("mobility\n");
						drawBits(BitBoard_getMobility(bitboard->stone[color], bitboard->stone[oppColor(color)]));
						getchar();
					}
					//printf("left:%d\n", left);
				}
				history[left] = color;
				left--;
			}
			else if (BitBoard_getMobility(bitboard->stone[oppColor(color)], bitboard->stone[color]) == 0) {
				break;
			}
			color = oppColor(color);
			Sleep(10);
			/*system("cls");
			BitBoard_Draw(bitboard, FALSE);*/
			//getchar();
		}
		printf("get result\n");
		result = (BitBoard_CountStone(bitboard->stone[BLACK]) - BitBoard_CountStone(bitboard->stone[WHITE]));
		for (j = left; j < 8; j++) {
			left++;
			BitBoard_Undo(bitboard);
		}
		for (j = BitBoard_CountStone(~(bitboard->stone[BLACK] | bitboard->stone[WHITE])); j < BOARD_SIZE * BOARD_SIZE - 12; j++) {
			left++;
			BitBoard_Undo(bitboard);
			if (history[left] == BLACK) {
				UpdateAllPattern(bitboard->stone[BLACK], bitboard->stone[WHITE], result, left);
			}
			else {
				BitBoard_AllOpp(bitboard);
				UpdateAllPattern(bitboard->stone[BLACK], bitboard->stone[WHITE], -result, left);
				BitBoard_AllOpp(bitboard);
			}
		}

		if ((i + 1) % 100 == 0) {
			printf("Learning %d / %d\n", i + 1, num);
			Pattern_Save();
		}
	}
	Pattern_Save();
	printf("exit\n");
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

#if __AVX2__
#elif __AVX__
	printf("\n########################################################################\n");
	printf("AVX�Ŏ��s���Ă��܂��B���W�X�^������Ȃ��Ȃ�G���[���o��\���������ł��B\n�ł������AVX2�ł̎��s���B\n");
	printf("########################################################################\n\n");
#endif



	printf("BOOK Path:");
	fgets(path, sizeof(path), stdin);
	strtok(path, "\n");
	fp = fopen(path, "r");

	if (!fp) {
		printf("can't open file : %s", path);
		return;
	}
	else {
		printf("Load File\n");
	}

	while (fgets(Line, sizeof(Line), fp)) {
		BitBoard_Reset(bitboard);
		left = 60;
		color = BLACK;

		printf("data:%d ", dataCount);

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

			/*
			BitBoard_Draw(bitboard, TRUE);
			getc(stdin);
			*/
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
			//printf("dataCount:%d randomTurn:%d left:%d diff:%d\n", dataCount, randomTurn, left, diff);
			//��������ł��Ȃ������ꍇ�p�X�Ƃ��ĐF��ς��Ē���
			if (BitBoard_Flip(bitboard, color, put) == 0) {
				color = oppColor(color);
				if (BitBoard_Flip(bitboard, color, put) == 0) {
					BitBoard_Draw(bitboard, TRUE);
					printf("turn:%d iligal data >> color:%c \"%c%c\"\n", 64-left, "BW"[color], c_pos[0], c_pos[1]);
					getc(stdin);
				}
			}
			/*
			if (left == 1) {
				BitBoard_Draw(bitboard, TRUE);
			}*/
			//getc(stdin);
			
			//�p�^�[���̕]���l�̍X�V
			if (color == BLACK) {
				UpdateAllPattern(bitboard->stone[BLACK], bitboard->stone[WHITE], diff, left);
			}
			else {
				BitBoard_AllOpp(bitboard);
				UpdateAllPattern(bitboard->stone[BLACK], bitboard->stone[WHITE], -diff, left);
				BitBoard_AllOpp(bitboard);
			}
			color = oppColor(color);
			left--;
		}
		//getc(stdin);
		dataCount++;
		//1000�Ɉ�x�\��
		if (dataCount % 100 == 0) {
			printf("Finish Reading : %d\n", dataCount);
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