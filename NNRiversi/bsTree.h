#pragma once
#include "BitBoard.h"

struct OpenData {
	uint64 bKey;
	uint64 wKey;
	int value;
}typedef OPdata;

struct OpenNode
{
	OpenNode *right;
	OpenNode *left;
	OPdata *data;
}typedef OPNode;

//探索木の要素を作成する
//戻り値:作成した要素
OPNode *OtreeCreate(OPdata* data);

char nodeKeyComp(OPdata *dat1, OPdata *dat2);

//戻り値:
//既にデータが存在する:0
//追加成功:1
int bsTree_add(OPNode *root, OPdata* data);

//木を全削除
void bsTree_Delete(OPNode *root);

//探索木探索
OPNode *bsTreeSearch(OPNode *root, OPdata *data);