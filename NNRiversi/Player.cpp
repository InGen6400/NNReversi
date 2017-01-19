#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Player.h"

void Player_Input(char *Input, int *x, int *y) {
	char cpy[100], *tmp;
	strcpy(cpy, Input);
	int ret = 0;

	tmp = strtok(cpy, " ,.-\t/:;");
	if (isalpha(tmp[0])) {
		*x = CharToPos(tmp[0]);
	}
	else if (isalnum(tmp[0])) {
		*y = tmp[0] - '0';
	}

	tmp = strtok(NULL, " ,.-\t/:;");
	if (isalpha(tmp[0])) {
		*x = CharToPos(tmp[0]);
	}
	else if (isalnum(tmp[0])) {
		*y = tmp[0] - '0';
	}
}

char CharToPos(char Input) {
	Input = tolower(Input);
	return Input - 'a' + 1;
}