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

//�T���؂̗v�f���쐬����
//�߂�l:�쐬�����v�f

char nodeKeyComp(const uint64 *bKey1, const uint64 *wKey1, const uint64 *bKey2, const uint64 *wKey2);

//�߂�l:
//���Ƀf�[�^�����݂���:0
//�ǉ�����:1
int bsTree_add(OpenTree *tree, const OKey *key, int value);

//�؂�S�폜
void bsTree_Delete(OPNode *root);

//�T���ؒT��
char bsTreeSearch(const OpenTree *tree, const OKey *key, int *value);

//�T���ؕۑ�(����ǂݍ��ݎ��̏��Ԃ͍�����̏��ɂȂ�Ȃ�)
void bsTreeSave(FILE *fp, const OPNode *root);