#pragma once
#include "bsTree.h"

#define OPEN_FILE_NAME "Resources\\book.txt"

//64[pos]x2[char] 二文字で1pos + 3空白 + 4評価値
#define OPFILE_LINE_SIZE (64*2 + 3 + 4)

OPNode *OPTree;

void open_read(BitBoard *board, OPNode *root);

void BitBoard_getKey(OPdata *data);
