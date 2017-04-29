#include "bsTree.h"
#include <stdio.h>

inline void OPdata_Init(OPdata *data, OKey key, int value) {
	data->key = key;
	data->value = value;
}

OPNode *OPNode_New() {
	OPNode *node = (OPNode*)malloc(sizeof(OPNode));
	if (node == NULL) {
		printf("can't allocate OPNode:%s", __LINE__);
		return NULL;
	}
	node->left = NULL;
	node->right = NULL;
	return node;
}

//既にデータが存在する:0 FALSE
//追加成功:1 TRUE
int bsTree_add(OPNode **root, OKey *key, int value) {
	OPNode** p = root;
	OPNode* newNode = NULL;
	while (*p != NULL) {
		//キーが同じ
		if ((*p)->data.key.b == key->b && (*p)->data.key.w == key->w) {
			(*p)->data.value = value;
			return FALSE;
		}
		//blackを比較して入力のほうが大きい　もしくは　blackが同じで入力のwhiteのほうが大きい
		else if ((*p)->data.key.b < key->b || ((*p)->data.key.b == key->b && (*p)->data.key.w < key->w)) {
			p = &(*p)->right;
		}
		else {
			p = &(*p)->left;
		}
	}
	newNode = OPNode_New();
	if (newNode == NULL) {
		printf("null node:%s", __LINE__);
		return FALSE;
	}
	OPdata_Init(&newNode->data, *key, value);
	*p = newNode;

	return TRUE;
}

void bsTree_Delete(OPNode *node) {
	if (node == NULL) {
		return;
	}
	bsTree_Delete(node->left);
	bsTree_Delete(node->right);
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

char bsTreeSearch(OPNode *root, OKey *key, int *value) {
	OPNode *p = root;

	while (p != NULL) {
		if (p->data.key.b == key->b && p->data.key.w == key->w) {
			*value = p->data.value;
			return TRUE;
		}
		else if ((p->data.key.b < key->b) && (p->data.key.b == key->b && p->data.key.w < key->w)) {
			p = p->right;
		}
		else {
			p = p->left;
		}
	}

	return FALSE;
}

void bsTreeSave(FILE *fp, const OPNode *root) {
	if (root == NULL) {
		printf("leaf\n");
		return;
	}
	bsTreeSave(fp, root->left);
	printf("save %d\n", root->data.value);
	fwrite(&(root->data), sizeof(OPdata), 1, fp);
	bsTreeSave(fp, root->right);
}