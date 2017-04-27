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

//�T���؂̗v�f���쐬����
//�߂�l:�쐬�����v�f
ONode *OtreeCreate(OPdata* data);

//�߂�l:
//���Ƀf�[�^�����݂���:0
//�ǉ�����:1
int bsTree_add(ONode *root, OPdata* data);

//�؂�S�폜
void bsTree_Delete(ONode *root);

//�T���ؒT��
ONode *bsTreeSearch(ONode *root, OPdata *data);