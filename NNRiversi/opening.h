#pragma once
#include "bsTree.h"

#define OPEN_TEXT_NAME "Resources\\book.txt"
#define OPEN_BIN_NAME "Resources\\opbook.bin"

//64[pos]x2[char] “ñ•¶Žš‚Å1pos + 3‹ó”’ + 4•]‰¿’l
#define OPFILE_LINE_SIZE (64*2 + 3 + 4)

OPNode *OPTree;

int open_Save();

void open_read_text(BitBoard *board, OPNode *root);

void BitBoard_getKey(OPdata *data);
