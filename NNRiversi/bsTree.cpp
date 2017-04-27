#include "bsTree.h"

ONode *OtreeCreate(OPdata* data) {
	ONode *tree = (ONode*)malloc(sizeof(ONode));
	tree->data = data;
	tree->left = NULL;
	tree->right = NULL;

	return tree;
}

//既にデータが存在する:0
//追加成功:1
int bsTree_add(ONode *root, OPdata* data) {
	ONode* p = root;
	while (p != NULL) {
		//キーが同じ
		if (p->data->bKey == data->bKey && p->data->wKey == data->wKey) {
			return 0;
		}
		//blackを比較して入力のほうが大きい　もしくは　blackが同じで入力のwhiteのほうが大きい
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