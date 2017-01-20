// NNRiversi.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
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
	char cpuTurn = -2, cpuPut = 0, turn = BLACK, flipCount = 0;
	char endFlag = FALSE;
	int x, y;

	printf("�ݒ�\n");
	//CPU�̐F�ݒ�
	while (cpuTurn == -2)
	{
		printf("CPU�̐F(B:black or W:white or N:NoCpu):");
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
			printf("�����ƁA�낤���o�O��Ƃ���ł����B���J�ɓ��͂��Ă���������Ƃ��ꂵ���ł��B\n W, B, N�̂ǂꂩ�ł���H\n");
		}
	}

	system("cls");
	mainBoard = Board_New();
	Board_Draw(mainBoard);
	while (!endFlag) {
		if (turn == cpuTurn) {
			//CPU�̃^�[��
			printf("CPU Thinking...");
			NegaMaxSearch(mainBoard, FALSE, turn, 0, &cpuPut);
			x = getX(cpuPut);
			y = getY(cpuPut);
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
					if (*(mainBoard->Sp - 1) != -2) {
						Board_Undo(mainBoard);
						Board_Undo(mainBoard);
						system("cls");
						printf("�߂��܂���\n");
						Board_Draw(mainBoard);
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
				break;
			}
		}

		if (x >= 1 && y >= 1 && x <= BOARD_SIZE && y <= BOARD_SIZE) {
			system("cls");
			if (flipCount = Board_Flip(mainBoard, turn, x, y) >= 1) {
				if (turn == cpuTurn) {
					printf("CPU Put (%c, %c)\n", "ABCDEFGH"[x - 1], "12345678"[y - 1]);
				}
				else {
					printf("You Put (%c, %c)\n", "ABCDEFGH"[x - 1], "12345678"[y - 1]);
				}
				printf("���Ԃ����ΐ�:%d\n", flipCount);
				turn = getOppStone(turn);
			}
			else {
				printf("You Can't Put (%c, %c)\n", "ABCDEFGH"[x - 1], "12345678"[y - 1]);
			}
			Board_Draw(mainBoard);

		}
		/*
		else 
		{
			if (Player_Input(tmp, &x, &y)) {
				
				}
				else {
					printf("You Can't Place that position\n");
				}
			}
			
		}*/
	}
	Board_Delete(mainBoard);
    return 0;
}

