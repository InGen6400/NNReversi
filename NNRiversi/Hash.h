#pragma once

typedef unsigned long long uint64;

#define HASH_LENGHT 1048576

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
	OpenNode *elmnt[HASH_LENGHT];
	int count;
	int collide;
}typedef OHash;

OHash *OHash_New();

void OHash_Add(OHash *hash, OKey *key, int value);

char OHash_Search(OHash *hash, OKey *key, int *value);

void Hash_Save(OHash *hash, FILE *fp);

char nodeKeyComp(OKey *key1, OKey *key2);