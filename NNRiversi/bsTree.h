#pragma once
#include "BitBoard.h"

struct OpenData {
	uint64 bKey;
	uint64 wKey;
	float value;
}typedef OPdata;

struct OpenNode
{
	ONode* right;
	ONode* left;
	OPdata data;
}typedef ONode;

typedef ONode* Otree;

Otree OtreeCreate(OPdata* data) {
	Otree tree = (Otree)malloc(sizeof(ONode));
	tree->data = *data;
	tree->left = NULL;
	tree->right = NULL;

	return tree;
}

//���Ƀf�[�^�����݂���:0
//�ǉ�����:1
int bsTree_add(Otree *tree, OPdata* data) {
	Otree* p = tree;
	while (*p != NULL) {
		//�L�[������
		if ((*p)->data.bKey == data->bKey && (*p)->data.wKey == data->wKey) {
			return 0;
		}
		//black���r���ē��͂̂ق����傫���@�������́@black�������œ��͂�white�̂ق����傫��
		else if ((*p)->data.bKey < data->bKey || ((*p)->data.bKey == data->bKey && (*p)->data.wKey < data->wKey)) {
			p = &(*p)->right;
		}
		else {
			p = &(*p)->left;
		}
	}

	*p = OtreeCreate(data);

	return 1;
}