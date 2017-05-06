#pragma once

extern struct OpenHash;
typedef struct OpenHash OHash;

#define OPEN_TEXT_NAME "Resources\\book.txt"
#define OPEN_BIN_NAME "Resources\\opbook.bin"

//64[pos]x2[char]+64 ‘SŠp•¶Žš1+”¼Šp‚Å1pos + 3‹ó”’ + 4•]‰¿’l + 10—\”õ
#define OP_LINE_SIZE (64*3 + 3 + 4+1000)

char open_Save(OHash *hash);

char open_Load(OHash *hash);

void open_read_text(OHash *hash);