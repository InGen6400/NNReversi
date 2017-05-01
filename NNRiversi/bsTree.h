#pragma once

#include "BitBoard.h"
#include <stdio.h>

struct OpenKey {
	uint64 b;
	uint64 w;
}typedef OKey;

struct OpenData {
	OpenKey key;
	int value;
}typedef OPdata;

struct OpenNode
{
	OpenNode *right;
	OpenNode *left;
	OPdata data;
}typedef OPNode;

struct OpenTree {
	OPNode *root;
	int count;
}typedef OpenTree;

//’Tõ–Ø‚Ì—v‘f‚ğì¬‚·‚é
//–ß‚è’l:ì¬‚µ‚½—v‘f

char nodeKeyComp(const uint64 *bKey1, const uint64 *wKey1, const uint64 *bKey2, const uint64 *wKey2);

//–ß‚è’l:
//Šù‚Éƒf[ƒ^‚ª‘¶İ‚·‚é:0
//’Ç‰Á¬Œ÷:1
int bsTree_add(OpenTree *tree, const OKey *key, int value);

//–Ø‚ğ‘Síœ
void bsTree_Delete(OPNode *root);

//’Tõ–Ø’Tõ
char bsTreeSearch(const OpenTree *tree, const OKey *key, int *value);

//’Tõ–Ø•Û‘¶(Ÿ‰ñ“Ç‚İ‚İ‚Ì‡”Ô‚Íª‚©‚ç‚Ì‡‚É‚È‚ç‚È‚¢)
void bsTreeSave(FILE *fp, const OPNode *root);