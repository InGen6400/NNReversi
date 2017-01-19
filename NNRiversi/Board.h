#pragma once

const short BOARD_SIZE = 8;
const short NUM_STONE = BOARD_SIZE * BOARD_SIZE;

const short STACK_STOP = -2;
const short STACK_BREAK = -1;

#pragma region piece_define

const short WALL = -1;
const short NONE = 0;
const short WHITE = 1;
const short BLACK  = 2;

#pragma endregion

#pragma region POS_define

const short A1 = 11;
const short A2 = 21;
const short A3 = 31;
const short A4 = 41;
const short A5 = 51;
const short A6 = 61;
const short A7 = 71;
const short A8 = 81;

const short B1 = 12;
const short B2 = 22;
const short B3 = 32;
const short B4 = 42;
const short B5 = 52;
const short B6 = 62;
const short B7 = 72;
const short B8 = 82;

const short C1 = 13;
const short C2 = 23;
const short C3 = 33;
const short C4 = 43;
const short C5 = 53;
const short C6 = 63;
const short C7 = 73;
const short C8 = 83;

const short D1 = 14;
const short D2 = 24;
const short D3 = 34;
const short D4 = 44;
const short D5 = 54;
const short D6 = 64;
const short D7 = 74;
const short D8 = 84;

const short E1 = 15;
const short E2 = 25;
const short E3 = 35;
const short E4 = 45;
const short E5 = 55;
const short E6 = 65;
const short E7 = 75;
const short E8 = 85;

const short F1 = 16;
const short F2 = 26;
const short F3 = 36;
const short F4 = 46;
const short F5 = 56;
const short F6 = 66;
const short F7 = 76;
const short F8 = 86;

const short G1 = 17;
const short G2 = 27;
const short G3 = 37;
const short G4 = 47;
const short G5 = 57;
const short G6 = 67;
const short G7 = 77;
const short G8 = 87;

const short H1 = 18;
const short H2 = 28;
const short H3 = 38;
const short H4 = 48;
const short H5 = 58;
const short H6 = 68;
const short H7 = 78;
const short H8 = 88;

#pragma endregion

struct _Board
{
	int Stone[(BOARD_SIZE + 2) * (BOARD_SIZE + 2)];
	int Stack[((BOARD_SIZE - 2) * 3 + 3)*BOARD_SIZE*BOARD_SIZE];
	int White, Black;
	int *Sp;
}typedef Board;

Board *Board_New(void);

void Board_Delete(Board *board);

void Board_Reset(Board *board);

void Board_Draw(Board *board);

int	Board_CountStone(const Board *board, int in_color);

int	Board_Flip(Board *board, char color, int x, int y);

int Board_FlipLine(Board *board, char color, int x, int y, int vec_x, int vec_y);

void Board_Undo(Board *board);

int	Board_CanFlip(const Board *board, int in_color, int x, int y);

int Board_CanFlipLine(const Board *board, char color, int x, int y, int vec_x, int vec_y);

int	Board_CountFlips(const Board *board, int in_color, int in_pos);

int ConvertPos(int x, int y);

int getX(int pos);

int getY(int pos);

int getOppStone(char color);

int Stack_POP(Board *board);

void Stack_PUSH(Board *board, int num);