#pragma once
#include "BitBoard.h"


struct TranceNode
{
	uint64 wKey;
	uint64 bKey;
	int max;
	int min;
}typedef TNode;

struct OpenNode
{
	uint64 wKey;
	uint64 bKey;
	float value;
}typedef ONode;