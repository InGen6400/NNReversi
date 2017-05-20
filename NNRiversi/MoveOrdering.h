#pragma once

#include "BitBoard.h"

struct PositionElement
{
	char pos;
	int value;
}typedef PElement;

struct PositionList
{
	PElement element[64];
	char count;
}typedef PList;

void PList_Init(PList *list, uint64 black, uint64 white, char left);

void PList_Sort(PList *list);