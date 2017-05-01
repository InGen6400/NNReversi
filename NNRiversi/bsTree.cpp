#include "bsTree.h"
#include <stdio.h>

inline void OPdata_Init(OPdata *data, const OKey *key, int value) {
	data->key = *key;
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
int bsTree_add(OpenTree *tree, const OKey *key, int value) {
	if (tree->root == NULL) {
		tree->root = OPNode_New();
		(tree->root)->data.value = value;
		(tree->root)->data.key = *key;
		return TRUE;
	}

	OPNode **n = &tree->root;
	while (*n != NULL) {
		//キーが同じ
		if ((*n)->data.key.b == key->b && (*n)->data.key.w == key->w) {
			(*n)->data.value = value;
			return FALSE;
		}
		//blackを比較して入力のほうが大きい　もしくは　blackが同じで入力のwhiteのほうが大きい
		else if ((*n)->data.key.b < key->b || ((*n)->data.key.b == key->b && (*n)->data.key.w < key->w)) {
			n = &(*n)->right;
		}
		else {
			n = &(*n)->left;
		}
	}
	*n = OPNode_New();
	if (n == NULL) {
		printf("null node:%dl", __LINE__);
		return FALSE;
	}
	OPdata_Init(&(*n)->data, key, value);

	return TRUE;
}

void bsTree_Delete(OPNode *node) {
	if (node == NULL) {
		return;
	}
	bsTree_Delete(node->left);
	node->left = NULL;
	bsTree_Delete(node->right);
	node->right = NULL;
	free(node);
}

char nodeKeyComp(const uint64 *bKey1, const uint64 *wKey1, const uint64 *bKey2, const uint64 *wKey2) {
	if (bKey1 == bKey2 && wKey1 == wKey2) {
		return 0;
	}
	if (bKey1 > bKey2) {
		return 1;
	}
	if (bKey1 == bKey2 && wKey1 > wKey2) {
		return 1;
	}

	if (bKey1 < bKey2) {
		return -1;
	}
	if (bKey1 == bKey2 && wKey1 < wKey2) {
		return -1;
	}
	printf("bsTree nodeKeyComp Bug\n");
	return 0;
}

char bsTreeSearch(const OpenTree *tree, const OKey *key, int *value) {
	OPNode *p = tree->root;

	while (p != NULL) {
		if (p->data.key.b == key->b && p->data.key.w == key->w) {
			*value = p->data.value;
			return TRUE;
		}
		else if ((p->data.key.b < key->b) || (p->data.key.b == key->b && p->data.key.w < key->w)) {
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
	fwrite(&(root->data.key.b), sizeof(uint64), 1, fp);
	fwrite(&(root->data.key.w), sizeof(uint64), 1, fp);
	fwrite(&(root->data.value), sizeof(int), 1, fp);
	bsTreeSave(fp, root->left);
	bsTreeSave(fp, root->right);
}