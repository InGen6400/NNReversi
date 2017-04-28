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

//�T���؂̗v�f���쐬����
//�߂�l:�쐬�����v�f
OPNode *OtreeCreate(OPdata* data);

char nodeKeyComp(OPdata *dat1, OPdata *dat2);

//�߂�l:
//���Ƀf�[�^�����݂���:0
//�ǉ�����:1
int bsTree_add(OPNode *root, OPdata* data);

//�؂�S�폜
void bsTree_Delete(OPNode *root);

//�T���ؒT��
OPNode *bsTreeSearch(OPNode *root, OPdata *data);