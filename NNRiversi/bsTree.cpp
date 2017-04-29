#include "bsTree.h"
#include <stdio.h>

OPdata *OPdata_New(const BitBoard *board, char color, int value) {
	OPdata *data = (OPdata*)malloc(sizeof(OPdata));
	data->value = value;
	BitBoard_getKey(board, color, &(data->bKey), &(data->wKey));
	return data;
}

OPNode *OPNode_New(OPdata* data) {
	OPNode *node = (OPNode*)malloc(sizeof(OPNode));
	node->data = (OPdata*)malloc(sizeof(OPdata));
	*(node->data) = *data;
	node->left = NULL;
	node->right = NULL;

	return node;
}

//既にデータが存在する:0 FALSE
//追加成功:1 TRUE
int bsTree_add(OPNode *root, OPdata* data) {
	OPNode* p = root;
	while (p != NULL) {
		//キーが同じ
		if (p->data->bKey == data->bKey && p->data->wKey == data->wKey) {
			return FALSE;
		}
		//blackを比較して入力のほうが大きい　もしくは　blackが同じで入力のwhiteのほうが大きい
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
	bsTree_Delete(node->left);
	bsTree_Delete(node->right);
	free(node->data);
	free(node);
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