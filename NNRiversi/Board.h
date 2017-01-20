#pragma once

const char BOARD_SIZE = 8;
const char NUM_STONE = BOARD_SIZE * BOARD_SIZE;

const char STACK_STOP = -2;
const char STACK_BREAK = -1;

#pragma region piece_define

const char WALL = -1;
const char NONE = 0;
const char WHITE = 1;
const char BLACK  = 2;

#pragma endregion

#pragma region POS_define

const char NOMOVE = -2;
const char PASS = -1;

const char A1 = 11;
const char A2 = 21;
const char A3 = 31;
const char A4 = 41;
const char A5 = 51;
const char A6 = 61;
const char A7 = 71;
const char A8 = 81;

const char B1 = 12;
const char B2 = 22;
const char B3 = 32;
const char B4 = 42;
const char B5 = 52;
const char B6 = 62;
const char B7 = 72;
const char B8 = 82;

const char C1 = 13;
const char C2 = 23;
const char C3 = 33;
const char C4 = 43;
const char C5 = 53;
const char C6 = 63;
const char C7 = 73;
const char C8 = 83;

const char D1 = 14;
const char D2 = 24;
const char D3 = 34;
const char D4 = 44;
const char D5 = 54;
const char D6 = 64;
const char D7 = 74;
const char D8 = 84;

const char E1 = 15;
const char E2 = 25;
const char E3 = 35;
const char E4 = 45;
const char E5 = 55;
const char E6 = 65;
const char E7 = 75;
const char E8 = 85;

const char F1 = 16;
const char F2 = 26;
const char F3 = 36;
const char F4 = 46;
const char F5 = 56;
const char F6 = 66;
const char F7 = 76;
const char F8 = 86;

const char G1 = 17;
const char G2 = 27;
const char G3 = 37;
const char G4 = 47;
const char G5 = 57;
const char G6 = 67;
const char G7 = 77;
const char G8 = 87;

const char H1 = 18;
const char H2 = 28;
const char H3 = 38;
const char H4 = 48;
const char H5 = 58;
const char H6 = 68;
const char H7 = 78;
const char H8 = 88;

#pragma endregion

struct _Board
{
	char Stone[(BOARD_SIZE + 2) * (BOARD_SIZE + 2)];
	char Stack[((BOARD_SIZE - 2) * 3 + 3)*BOARD_SIZE*BOARD_SIZE];
	char White, Black;
	char *Sp;
}typedef Board;

Board *Board_New(void);

void Board_Delete(Board *board);

void Board_Reset(Board *board);

void Board_Draw(Board *board);

char Board_CountStone(const Board *board, char in_color);

char Board_Flip(Board *board, char color, char x, char y);

char Board_FlipLine(Board *board, char color, char x, char y, char vec_x, char vec_y);

void Board_Undo(Board *board);

char Board_CanFlip(const Board *board, char color, char x, char y);

char Board_CanFlipLine(const Board *board, char color, char x, char y, char vec_x, char vec_y);

char Board_CountFlips(const Board *board, char in_color, char in_pos);

char ConvertPos(char x, char y);

char getX(char pos);

char getY(char pos);

char getOppStone(char color);

char Stack_POP(Board *board);

void Stack_PUSH(Board *board, char num);