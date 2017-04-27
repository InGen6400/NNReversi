#include "bsTree.h"

ONode *OtreeCreate(OPdata* data) {
	ONode *tree = (ONode*)malloc(sizeof(ONode));
	tree->data = data;
	tree->left = NULL;
	tree->right = NULL;

	return tree;
}

//���Ƀf�[�^�����݂���:0
//�ǉ�����:1
int bsTree_add(ONode *root, OPdata* data) {
	ONode* p = root;
	while (p != NULL) {
		//�L�[������
		if (p->data->bKey == data->bKey && p->data->wKey == data->wKey) {
			return 0;
		}
		//black���r���ē��͂̂ق����傫���@�������́@black�������œ��͂�white�̂ق����傫��
		else if (p->data->bKey < data->bKey || (p->data->bKey == data->bKey && p->data->wKey < data->wKey)) {
			p = p->right;
		}
		else {
			p = p->left;
		}
	}
	p = OtreeCreate(data);

	return 1;
}

void bsTree_Delete(ONode *data) {
	if (data == NULL) {
		return;
	}
	bsTree_Delete(data->left);
	bsTree_Delete(data->right);
	free(data);
}