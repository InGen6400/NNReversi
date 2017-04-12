#pragma once
#include "BitBoard.h"

typedef unsigned long long uint64;

struct charNode{
	char value;
	charNode *next;
	charNode *prev;
};

typedef struct BookData {
	int blackScore;
	int whiteScore;
	short bestMove;
	short bestScore;
}BookData;

typedef struct BookNode {
	BookNode *next;
	uint64 keyB;
	uint64 keyW;
	BookData *bookData;
}BookNode;
/*
BookNode **BookTable_Init();

void BookList_Add(BookNode *prev, BookNode *node);

void BookList_Del(BookNode *node);
*/
charNode *CharListInit(int num);

void addNode(charNode *node);

void removeNode(charNode *node);