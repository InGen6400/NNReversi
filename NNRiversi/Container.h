#pragma once

struct charNode{
	char value;
	charNode *next;
	charNode *prev;
};

charNode *CharListInit(int num);

void addNode(charNode *node);

void removeNode(charNode *node);