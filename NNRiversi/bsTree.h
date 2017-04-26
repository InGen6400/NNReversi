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
	OPdata data;
}typedef ONode;

typedef ONode* Otree;

//�T���؂̗v�f���쐬����
//�߂�l:�쐬�����v�f
Otree OtreeCreate(OPdata* data);

//�߂�l:
//���Ƀf�[�^�����݂���:0
//�ǉ�����:1
int bsTree_add(Otree *tree, OPdata* data);