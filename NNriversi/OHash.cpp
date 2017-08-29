#include "OHash.h"
#include "const.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <intrin.h>

#define FNV_OFFSET_BIAS_32 2166136261
#define FNV_PRIME_32 16777619

typedef unsigned int uint32;

inline char nodeKeyComp(OKey key1, OKey key2) {
	if (key1.b == key2.b && key1.w == key2.w) {
		return 0;
	}
	if (key1.b > key2.b) {
		return 1;
	}
	if (key1.b == key2.b && key1.w > key2.w) {
		return 1;
	}

	if (key1.b < key2.b) {
		return -1;
	}
	if (key1.b == key2.b && key1.w < key2.w) {
		return -1;
	}
	return 0;
}

inline uint32 getHash(OKey key) {
	uint32 hash = FNV_OFFSET_BIAS_32;
	__m128i key128 = _mm_setr_epi64x(key.b, key.w);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 0);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 1);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 2);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 3);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 4);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 5);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 6);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 7);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 8);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 9);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 10);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 11);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 12);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 13);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 14);
	hash = (FNV_PRIME_32 * hash) ^ _mm_extract_epi8(key128, 15);
	return hash&(HASH_LENGHT-1);
}

void OPNode_Init(OPNode *node, OKey *key, int value) {
	node->next = NULL;
	node->value = value;
	node->key = *key;
}

OPNode *OPNode_New(OpenKey *key, int value) {
	OPNode *node;
	node = (OPNode*)malloc(sizeof(OPNode));
	if (node == NULL) {
		printf("can't alloc node:Hash.cpp %d", __LINE__);
		return NULL;
	}
	OPNode_Init(node, key, value);
	return node;
}

void OHash_Init(OHash *opHash) {
	for (int i = 0; i < HASH_LENGHT; i++) {
		opHash->elmnt[i] = NULL;
	}
	opHash->count = 0;
	opHash->collide = 0;
}

OHash *OHash_New() {
	OHash *opHash;
	opHash = (OHash*)malloc(sizeof(OHash));
	if (opHash == NULL) {
		return NULL;
	}
	OHash_Init(opHash);
	return opHash;
}

void OHash_Add(OHash *opHash, OKey *key, int value) {
	unsigned int index;
	OPNode *node;
	OPNode *prev;
	index =	getHash(*key);
	node = opHash->elmnt[index];
	if (opHash->elmnt[index] != NULL) {
		while (node != NULL) {
			if (nodeKeyComp(*key, node->key) == 0) {
				node->value = value;
				return;
			}
			prev = node;
			node = node->next;
		}
		prev->next = OPNode_New(key, value);
		opHash->count++;
		opHash->collide++;
	}
	else {
		opHash->elmnt[index] = OPNode_New(key, value);
		opHash->count++;
	}
}

void OHash_DeleteAll(OHash *opHash) {
	int i;
	for (i = 0; i < HASH_LENGHT; i++) {
		OPNode *node;
		while (opHash->elmnt[i] != NULL) {
			node = opHash->elmnt[i]->next;
			free(opHash->elmnt[i]);
			opHash->elmnt[i] = node;
			opHash->count--;
			opHash->collide--;
		}
	}
	opHash->collide = 0;
}

char OHash_Search(OHash *opHash, OKey *key, int *value) {
	int index = getHash(*key);
	OPNode *node = opHash->elmnt[index];
	while (node != NULL) {
		if (nodeKeyComp(*key, node->key) == 0) {
			*value = node->value;
			return TRUE;
		}
		node = node->next;
	}
	return FALSE;
}

void OHash_Save(OHash *opHash, FILE *fp) {
	int i;
	for (i = 0; i < HASH_LENGHT; i++) {
		OPNode *node = opHash->elmnt[i];
		while (node != NULL) {
			fwrite(&node->key, sizeof(OKey), 1, fp);
			fwrite(&node->value, sizeof(int), 1, fp);
			node = node->next;
		}
	}
}
