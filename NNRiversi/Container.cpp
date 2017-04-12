#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "Container.h"

charNode *CharListInit(int num) {
	charNode *ret;
	ret = (charNode*)malloc(sizeof(charNode) * num);
	return ret;
}

void BookList_Add(BookNode *prev, BookData *data, uint64 key1, uint64 key2) {
	
}

//void BookList_Del(BookNode *node);

void addNode(charNode *node) {
	if (node->next != NULL) {
		node->next->prev = node;
	}
	if (node->prev != NULL) {
		node->prev->next = node;
	}
}

void removeNode(charNode *node) {
	if (node->next != NULL) {
		node->next->prev = node->prev;
	}
	if (node->prev != NULL) {
		node->prev->next = node->next;
	}
}