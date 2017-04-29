#pragma once

#include "bsTree.h"

#define OPEN_TEXT_NAME "Resources\\book.txt"
#define OPEN_BIN_NAME "Resources\\opbook.bin"

//64[pos]x2[char]+64 �S�p����1+���p��1pos + 3�� + 4�]���l + 10�\��
#define OP_LINE_SIZE (64*3 + 3 + 4+1000)

char open_Save();

void open_read_text();

void open_find(const BitBoard *board, char color, OPdata *data);