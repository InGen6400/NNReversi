#pragma once
#include "BitBoard.h"

struct OpenData {
	uint64 bKey;
	uint64 wKey;
	float value;
}typedef OPdata;

struct OpenNode
{
	OpenNode *right;
	OpenNode *left;
	OPdata *data;
}typedef ONode;

//探索木の要素を作成する
//戻り値:作成した要素
ONode *OtreeCreate(OPdata* data);

//戻り値:
//既にデータが存在する:0
//追加成功:1
int bsTree_add(ONode *root, OPdata* data);

//木を全削除
void bsTree_Delete(ONode *root);

//探索木探索
ONode *bsTreeSearch(ONode *root, OPdata *data);