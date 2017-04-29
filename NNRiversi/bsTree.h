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

//探索木の要素を作成する
//戻り値:作成した要素

char nodeKeyComp(uint64 *bKey1, uint64 *wKey1, uint64 *bKey2, uint64 *wKey2);

//戻り値:
//既にデータが存在する:0
//追加成功:1
int bsTree_add(OPNode **root, OKey *key, int value);

//木を全削除
void bsTree_Delete(OPNode *root);

//探索木探索
char bsTreeSearch(OPNode *root, OKey *key, int *value);

//探索木保存(次回読み込み時の順番は根からの順にならない)
void bsTreeSave(FILE *fp, const OPNode *root);