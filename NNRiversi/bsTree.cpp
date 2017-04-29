#include "bsTree.h"
#include <stdio.h>

OPNode *OtreeCreate(OPdata* data) {
	OPNode *node = (OPNode*)malloc(sizeof(OPNode));
	node->data = data;
	node->left = NULL;
	node->right = NULL;

	return node;
}

//���Ƀf�[�^�����݂���:0 FALSE
//�ǉ�����:1 TRUE
int bsTree_add(OPNode *root, OPdata* data) {
	OPNode* p = root;
	while (p != NULL) {
		//�L�[������
		if (p->data->bKey == data->bKey && p->data->wKey == data->wKey) {
			return FALSE;
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

	return TRUE;
}

void bsTree_Delete(OPNode *data) {
	if (data == NULL) {
		return;
	}
	bsTree_Delete(data->left);
	bsTree_Delete(data->right);
	free(data);
}

char nodeKeyComp(OPdata *dat1, OPdata *dat2) {
	if (dat1->bKey == dat2->bKey && dat1->wKey == dat2->wKey) {
		return 0;
	}
	if (dat1->bKey > dat2->bKey) {
		return 1;
	}
	if (dat1->bKey == dat2->bKey && dat1->wKey > dat2->wKey) {
		return 1;
	}

	if (dat1->bKey < dat2->bKey) {
		return -1;
	}
	if (dat1->bKey == dat2->bKey && dat1->wKey < dat2->wKey) {
		return -1;
	}
	printf("bsTree nodeKeyComp Bug\n");
}

OPdata *bsTreeSearch(OPNode *root, OPdata *data) {
	OPNode *p = root;

	while (p != NULL) {
		if (p->data->bKey == data->bKey && p->data->wKey == data->wKey) {
			return p->data;
		}
		else if ((p->data->bKey < data->bKey) && (p->data->bKey == data->bKey && p->data->wKey < data->wKey)) {
			p = p->right;
		}
		else {
			p = p->left;
		}
	}

	return NULL;
}

int bsTreeSave(FILE *fp, const OPNode *root) {
	if (root == NULL) {
		return;
	}
	bsTreeSave(fp, root->right);
	bsTreeSave(fp, root->left);
	fwrite(root->data, sizeof(OPdata), 1, fp);
}