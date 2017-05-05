#pragma once
#include <stdio.h>

typedef unsigned long long uint64;

#define HASH_LENGHT (1048576)

struct OpenKey {
	uint64 b;
	uint64 w;
}typedef OKey;

struct OpenNode
{
	OpenNode *next;
	OpenKey key;
	int value;
}typedef OPNode;

struct OpenHash {
	OPNode *elmnt[HASH_LENGHT];
	int count;
	int collide;
}typedef OHash;

OHash *OHash_New();

void OHash_Add(OHash *hash, OKey *key, int value);

void OHash_DeleteAll(OHash *hash);

char OHash_Search(OHash *hash, OKey *key, int *value);

void OHash_Save(OHash *hash, FILE *fp);