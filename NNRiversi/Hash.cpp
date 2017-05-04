#include "Hash.h"
#include "const.h"
#include <stdlib.h>
#include <stdio.h>

OPNode *OPNode_New(OpenKey *key, int value) {
	OPNode *node = (OPNode*)malloc(sizeof(OPNode));
	if (node == NULL) {
		return NULL;
	}
	node->next = NULL;
	node->value = value;
	node->key = *key;
}

inline unsigned int getHash(OKey key) {
	return (key.b + key.w)&(HASH_LENGHT-1);
}

void OHash_Init(OHash *hash) {
	for (int i = 0; i < HASH_LENGHT; i++) {
		hash->elmnt[i] = NULL;
	}
	hash->count = 0;
	hash->collide = 0;
}

OHash *OHash_New() {
	OHash *hash = (OHash*)malloc(sizeof(OHash));
	if (hash == NULL) {
		return NULL;
	}
	OHash_Init(hash);
	return hash;
}

void OHash_Add(OHash *hash, OKey *key, int value) {
	unsigned int index = getHash(*key);
	volatile OPNode *node = hash->elmnt[index];
	if (node != NULL) {
		while (node->next != NULL && nodeKeyComp(key, (OKey*)&(node->key)) < 0) {
			node = node->next;
		}
		node->next = OPNode_New(key, value);
		hash->count++;
		hash->collide++;
	}
	else {
		hash->elmnt[index] = OPNode_New(key, value);
		hash->count++;
	}
}

char OHash_Search(OHash *hash, OKey *key, int *value) {
	int index = getHash(*key);
	volatile OPNode *node = hash->elmnt[index];
	if (node != NULL) {
		while (nodeKeyComp(key, (OKey*)&(node->key)) > 0) {
			node = node->next;
		}
		if (nodeKeyComp(key, (OKey*)&(node->key)) == 0) {
			*value = node->value;
			return TRUE;
		}
	}
	return FALSE;
}

void Hash_Save(OHash *hash, FILE *fp) {
	int i;
	for (i = 0; i < HASH_LENGHT; i++) {
		OPNode *node = hash->elmnt[i];
		while (node != NULL) {
			fwrite(&node->key, sizeof(OKey), 1, fp);
			fwrite(&node->value, sizeof(int), 1, fp);
			node = node->next;
		}
	}
}

char nodeKeyComp(OKey *key1, OKey *key2) {
	if (key1->b == key2->b && key1->w == key2->w) {
		return 0;
	}
	if (key1->b > key2->b) {
		return 1;
	}
	if (key1->b == key2->b && key1->w > key2->w) {
		return 1;
	}

	if (key1->b < key2->b) {
		return -1;
	}
	if (key1->b == key2->b && key1->w < key2->w) {
		return -1;
	}
	return 0;
}