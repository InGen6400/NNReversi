#include "bsTree.h"
#include <stdio.h>

OPdata *OPdata_New() {
}

OPNode *OPNode_New() {
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
	p = OPNode_New(data);

	return TRUE;
}

void bsTree_Delete(OPNode *node) {
	if (node == NULL) {
		return;
	}
}

char nodeKeyComp(uint64 *bKey1, uint64 *wKey1, uint64 *bKey2, uint64 *wKey2) {
	if (bKey1 == bKey2 && bKey1 == bKey2) {
		return 0;
	}
	if (bKey1 > bKey2) {
		return 1;
	}
	if (bKey1 == bKey2 && bKey1 > bKey2) {
		return 1;
	}

	if (bKey1 < bKey2) {
		return -1;
	}
	if (bKey1 == bKey2 && bKey1 < bKey2) {
		return -1;
	}
	printf("bsTree nodeKeyComp Bug\n");
	return 0;
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

void bsTreeSave(FILE *fp, const OPNode *root) {
	if (root == NULL) {
		printf("leaf\n");
		return;
	}
	bsTreeSave(fp, root->right);
	bsTreeSave(fp, root->left);
	fwrite(root->data, sizeof(OPdata), 1, fp);
}