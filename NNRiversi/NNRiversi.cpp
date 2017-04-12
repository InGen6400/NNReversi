// NNRiversi.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include <stdio.h>
#include "BitBoard.h"
#include "Player.h"
#include "const.h"
//#include "CPU.h"
#include "Hive_routine.h"
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

//void Game_Time(char showMobility);

//void MODE_DEBUG();

void MODE_READBOOK();

void MODE_ResetReadEnd();

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


//AVX2が利用可能かどうかの判定
	int CPUInfo[4];
	__cpuidex(CPUInfo, 7, 0);
	if (CPUInfo[1] & (1 << 5)) {
		printf("CPUInfo[1]:%d\nAVX2が利用可能です。\n高速モードで起動します。\n", CPUInfo[1]);
		AVX2_FLAG = TRUE;
	}
	else {
		printf("CPUInfo[1]:%d\nAVX2は利用'不'可能です。\n低速モードで起動します。\n", CPUInfo[1]);
		AVX2_FLAG = FALSE;
	}
	Pattern_setAVX(AVX2_FLAG);
	putchar('\n');
	Pattern_Init();
	//Pattern_Save();
	Pattern_Load();

	Board_InitConst();

	printf("\n設定\n");


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
		else if (tmp[0] == 'R' || tmp[0] == 'r') {
			mode = READ;
		}
		else if (tmp[0] == '\\') {
			MODE_ResetReadEnd();
		}
		else if (tmp[0] == '.') {
			showMobility = TRUE;
			printf("着手可能な場所を表示します\n");
		}
		else if (tmp[0] == '@' && tmp[1] == 'D') {
			mode = DEBUG;
		}
		else {
			printf("そのようなモードは存じておりません\n");
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
		//Game_Time(FALSE);
	}
	else if (mode == DEBUG) {
		//MODE_DEBUG();
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
				//プレイヤーのターン
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
			else {
				//プレイヤーのターン
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

	Hive *hive = Hive_New();
	if (AVX2_FLAG == TRUE) {
		setLevel(hive, 9, 18);
	}
	else {
		setLevel(hive, 7, 18);
	}
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
		if ((bitboard->stone[BLACK] & bitboard->stone[WHITE]) != 0) {
			
			drawBits(bitboard->stone[BLACK] & bitboard->stone[WHITE]);
		}
		if (BitBoard_getMobility(bitboard->stone[color], (bitboard->stone[oppColor(color)])) > 0) {
			passed = FALSE;
			if (color == cpuTurn) {
				//CPUのターン
				printf("CPU Thinking...\n");
				printf("value:%d\n", NextMove(hive, bitboard, color, &put));
				getXY(put, &x, &y);
			}
			else {
				//プレイヤーのターン
				while (1) {
					printf("あなたの番です:");
					fgets(tmp, sizeof(tmp), stdin);
					if (tmp[0] == 'q') {
						endFlag = TRUE;
						break;
					}
					else if (tmp[0] == '.' && tmp[1] == '.') {
						if (*(bitboard->Sp - 1) != 0xFFFFFFFFFFFFFFFFULL) {
							//前回のCPUのターンまで戻る
							do {
								if (*(bitboard->Sp - 1) == 0xFFFFFFFFFFFFFFFFULL) {
									break;
								}
								color = BitBoard_Undo(bitboard);
							} while (color == cpuTurn);
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

			//system("cls");
			if ((BitBoard_getMobility(bitboard->stone[color], bitboard->stone[oppColor(color)]) & put) != 0) {
				if (BitBoard_Flip(bitboard, color, put) >= 1) {
					//system("cls");
					BitBoard_Draw(bitboard, showMobility);
					if (color == cpuTurn) {
						printf("CPU Put (%c, %c)\n", "HGFEDCBA"[x], "87654321"[y]);
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
	Hive_Del(hive);
}
/*Hive 未対応
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

	//CPUの色設定
	system("cls");
	BitBoard_Draw(bitboard, showMobility);
	while (!endFlag) {
		if (BitBoard_getMobility(bitboard->stone[turn], (bitboard->stone[oppColor(turn)])) > 0) {
			passed = FALSE;
			if (turn == cpuTurn) {
				//CPUのターン
				cpu->leaf = 0;
				CPU_Move(cpu, bitboard, &put, turn, left);
				getXY(put, &x, &y);
			}
			else {
				//CPU2のターン
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
*/
/*hive 未対応
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

	//CPUの色設定
	system("cls");
	BitBoard_Draw(bitboard, FALSE);
	while (!endFlag) {
		if (BitBoard_getMobility(bitboard->stone[turn], (bitboard->stone[oppColor(turn)])) > 0) {
			passed = FALSE;
			if (turn == cpuTurn) {
				//CPUのターン
				cpu->node = 0;
				CPU_PUT(cpu, &put, turn, left);
				getXY(put, &x, &y);
			}
			else {
				//CPU2のターン
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
	
	put=getPos_book_upper("D3");

	cpu2->end = timeGetTime();
	cpu->end = timeGetTime();
	printf("Time:%.4f", ((cpu2->end - cpu2->start)) / 1000.0);
	BitBoard_Delete(bitboard);
	CPU_Delete(cpu);
	CPU_Delete(cpu2);
}
*/
inline int get_rand(int max)
{
	return (int)((double)max * rand() / (RAND_MAX + 1.0));
}


void MODE_LEARN() {
	BitBoard *bitboard = BitBoard_New();
	Hive *hive = Hive_New();
	uint64 move;
	int history[BITBOARD_SIZE * BITBOARD_SIZE];
	int i, j, num, turn, value;
	int left;
	char color;
	char ShowLearn = 'n';
	int result;
	srand((unsigned)time(NULL));
	printf("対戦回数:");
	scanf("%d", &num);

	printf("観戦しますか？(y/n):");
	scanf("%s", &ShowLearn);
	while (ShowLearn != 'n' && ShowLearn != 'y') {
		printf("yかnで入力してください\n");
		printf("観戦しますか？(y/n):");
		scanf("%s", &ShowLearn);
	}
	if (AVX2_FLAG == TRUE) {
		setLevel(hive, 4, 12);
	}
	else {
		setLevel(hive, 4, 12);
	}
	getchar();

	printf("\n0%%                      50%%                      100%%\n");
	printf("+-----------------------+------------------------+\n");

	for (i = 0; i < num; i++) {
		BitBoard_Reset(bitboard);
		color = BLACK;
		left = 60;
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
		while (1) {
			if (BitBoard_getMobility(bitboard->stone[color], bitboard->stone[oppColor(color)]) > 0) {
				if (left > 12 && get_rand(100) < 1) {
					move_random(bitboard, color);
				}
				else {
					value = NextMove(hive, bitboard, color, &move);
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
			if (ShowLearn == 'y') {
				system("cls");
				BitBoard_Draw(bitboard, FALSE);
			}
			//getchar();
		}
		result = (BitBoard_CountStone(bitboard->stone[BLACK]) - BitBoard_CountStone(bitboard->stone[WHITE]));
		for (j = left; j < 8; j++) {
			left++;
			BitBoard_Undo(bitboard);
		}
		for (j = BitBoard_CountStone(~(bitboard->stone[BLACK] | bitboard->stone[WHITE])); j < BITBOARD_SIZE * BITBOARD_SIZE - 12; j++) {
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
		putchar('\r');
		for (j = 0; j < (i + 1) / (num / 50); j++) {
			putchar('#');
		}
		printf("\t\t\t\t\t\t\t%d/%d", i+1, num);
		if (i % 100 == 0) {
			Pattern_Save();
			printf("\tsaved:%d", i);
		}
		//printf("<<<%3d%%", (double)i / num * 100);
	}
	printf("\n");
	if (Pattern_Save() == 1) {
		printf("Save success\n");
	}
	printf("exit\n");
}

void MODE_READBOOK() {
	//ファイルパスと読み込み後のファイルパス
	char path[1000], loadedPath[1000];
	//一行分の文字列
	char Line[LINE_MAX];
	//着手位置だけのデータ
	char *positions;
	//着手データの一次記憶変数, 現在手番の色
	char c_pos[5], color=BLACK;
	//イテレータ, ターン数, 石差, 読み込んだ行数
	int i, left=60, diff, dataCount = 0, randomTurn;
	FILE *fp;
	BitBoard *bitboard = BitBoard_New();
	//着手位置
	uint64 put;

#if __AVX2__
#elif __AVX__
	printf("\n########################################################################\n");
	printf("AVXで実行しています。レジスタが足りなくなりエラーが出る可能性が高いです。\nできる限りAVX2での実行を。\n");
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

	char skip;
	while (fgets(Line, sizeof(Line), fp)) {
		BitBoard_Reset(bitboard);
		left = 60;
		color = BLACK;
		printf("data:%d ", dataCount);
		skip = FALSE;
		//correctbkは通し番号があるのでカット
		//strtok(Line, " ");
		//序盤のランダムは無視しターンカウントだけ進める
		positions = strtok(Line, " ");
		for (i = 0; positions[i] != '\0'; i += 2) {
			c_pos[0] = positions[i];
			c_pos[1] = positions[i + 1];
			put = getPos_book_upper(c_pos);
			//もし着手できなかった場合パスとして色を変えて着手
			if (BitBoard_Flip(bitboard, color, put) == 0) {
				color = oppColor(color);
				if (BitBoard_Flip(bitboard, color, put) == 0) {
					printf("iligal data >> skip read RANDOM\n");
					skip = TRUE;
					break;
				}
			}
			left--;

			/*
			BitBoard_Draw(bitboard, TRUE);
			getc(stdin);
			*/
			//パターンの更新はしない
			color = oppColor(color);
		}
		if (skip == FALSE) {
			randomTurn = left;
			//空白以降の棋譜を取得
			positions = strtok(NULL, " ");
			printf("positions : %s\n", positions);
			//石差取得(黒視点)
			diff = atoi(strtok(NULL, " "));

			for (i = 0; positions[i] != NULL; i += 2) {
				//二文字だけ取り出す(A1 etc...)
				c_pos[0] = positions[i];
				c_pos[1] = positions[i + 1];
				put = getPos_book_upper(c_pos);
				//printf("dataCount:%d randomTurn:%d left:%d diff:%d\n", dataCount, randomTurn, left, diff);
				//もし着手できなかった場合パスとして色を変えて着手
				if (BitBoard_Flip(bitboard, color, put) == 0) {
					color = oppColor(color);
					if (BitBoard_Flip(bitboard, color, put) == 0) {
						BitBoard_Draw(bitboard, TRUE);
						printf("turn:%d iligal data >> color:%c \"%c%c\"\n", 64 - left, "BW"[color], c_pos[0], c_pos[1]);
						skip = TRUE;
						break;
					}
				}
				/*
				if (left == 1) {
				BitBoard_Draw(bitboard, TRUE);
				}*/
				//getc(stdin);

				color = oppColor(color);
				left--;
				//パターンの評価値の更新
				if (skip == FALSE) {
					if (color == BLACK) {
						UpdateAllPattern(bitboard->stone[BLACK], bitboard->stone[WHITE], diff, left);
					}
					else {
						UpdateAllPattern(bitboard->stone[WHITE], bitboard->stone[BLACK], -diff, left);
					}
				}
			}
			//getc(stdin);
			dataCount++;
			//1000に一度表示
			if (dataCount % 100 == 0) {
				printf("Finish Reading : %d\n", dataCount);
			}
		}
		else {
			skip = FALSE;
		}
	}

	fclose(fp);

	Pattern_Save();


	//fgetsの改行を無視
	strtok(path, "\n");

	printf("asd\n");

	//ファイル名を変更
	sprintf(loadedPath, "%s.end", path);
	if (rename(path, loadedPath) == 0) {
		printf("ファイル名変更:%s->%s\n", path, loadedPath);
	}
}

void MODE_ResetReadEnd() {
	char tmp[1000];
	char cmd[1100];
	printf(".endファイルのあるディレクトリパス:");
	scanf("%s", &tmp);
	sprintf(cmd, "ren %s\\*.end *.", tmp);
	system(cmd);
	fgets(tmp, sizeof(tmp), stdin);
	printf("\n");
}