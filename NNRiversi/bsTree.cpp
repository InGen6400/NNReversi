#include "bsTree.h"
#include <stdio.h>

inline void OPdata_Init(OPdata *data, OKey key, int value) {
	data->key = key;
	data->value = value;
}

OPNode *OPNode_New() {
	OPNode *node;
	node = (OPNode*)malloc(sizeof(OPNode));
	if (node == NULL) {
		printf("can't allocate OPNode:%dl", __LINE__);
		return NULL;
	}
	node->left = NULL;
	node->right = NULL;
	node->data.value = 0;
	return node;
}

//既にデータが存在する:0 FALSE
//追加成功:1 TRUE
int bsTree_add(OPNode **root, OKey *key, int value) {
	if (*root == NULL) {
		*root = OPNode_New();
		(*root)->data.value = value;
		(*root)->data.key = *key;
		printf("new\n");
		return TRUE;
	}
	OPNode *p = NULL;
	OPNode *n = *root;
	while (n != NULL) {
		p = n;
		//キーが同じ
		if (n->data.key.b == key->b && n->data.key.w == key->w) {
			n->data.value = value;
			printf("same\n");
			return FALSE;
		}
		//blackを比較して入力のほうが大きい　もしくは　blackが同じで入力のwhiteのほうが大きい
		else if (n->data.key.b < key->b || (n->data.key.b == key->b && n->data.key.w < key->w)) {
			n = n->right;
		}
		else {
			n = n->left;
		}
	}
	n = OPNode_New();
	if (n == NULL) {
		printf("null node:%dl", __LINE__);
		return FALSE;
	}
	OPdata_Init(&n->data, *key, value);
	if (p->data.key.b < key->b || (p->data.key.b == key->b && p->data.key.w < key->w)) {
		p->right = n;
	}
	else {
		p->left = n;
	}

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
		//printf("leaf\n");
		return;
	}
	bsTreeSave(fp, root->left);
	fwrite(&(root->data), sizeof(OPdata), 1, fp);
	bsTreeSave(fp, root->right);
}