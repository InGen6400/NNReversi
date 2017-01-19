#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Player.h"
#include "const.h"

int Player_Input(char *Input, int *x, int *y) {
	char cpy[100], *tmp;
	strcpy(cpy, Input);
	int ret = 0;

	if (strlen(cpy) >= 3) {
		tmp = strtok(cpy, " ,.-\t/:;");
		if ((tmp[0] >= 'a' && tmp[0] <= 'h') || (tmp[0] >= 'A' && tmp[0] <= 'H')) {
			*x = CharToPos(tmp[0]);
		}
		else if (isalnum(tmp[0])) {
			*y = tmp[0] - '0';
		}
		else {
			return FALSE;
		}

		tmp = strtok(NULL, " ,.-\t/:;");
		if ((tmp[0] >= 'a' && tmp[0] <= 'h') || (tmp[0] >= 'A' && tmp[0] <= 'H')) {
			*x = CharToPos(tmp[0]);
		}
		else if (isalnum(tmp[0])) {
			*y = tmp[0] - '0';
		}
		else {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}
}

char CharToPos(char Input) {
	Input = tolower(Input);
	return Input - 'a' + 1;
}