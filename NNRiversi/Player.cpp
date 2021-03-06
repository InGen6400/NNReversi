#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Player.h"
#include "const.h"

//入力のx,yを判断して適切に変換して返す
int Player_Input(char *Input, int *x, int *y) {
	char cpy[100], *tmp;
	strcpy(cpy, Input);
	int ret = 0;

	if (strlen(cpy) == 4) {
		tmp = strtok(cpy, " ,.-\t/:;");
		if ((tmp[0] >= 'a' && tmp[0] <= 'h') || (tmp[0] >= 'A' && tmp[0] <= 'H')) {
			*x = CharToPos(tmp[0]);
		}
		else if (tmp[0] >= '1' && tmp[0] <= '8') {
			*y = '8' - tmp[0];
		}
		else {
			return FALSE;
		}

		if ((tmp = strtok(NULL, " ,.-\t/:;")) == '\0') {
			return FALSE;
		}
		if ((tmp[0] >= 'a' && tmp[0] <= 'h') || (tmp[0] >= 'A' && tmp[0] <= 'H')) {
			*x = CharToPos(tmp[0]);
		}
		else if (tmp[0] >= '1' && tmp[0] <= '8') {
			*y = '8' - tmp[0];
		}
		else {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}
	return TRUE;
}

//A~H を 1~8にする
char CharToPos(char Input) {
	Input = (char)tolower(Input);
	return 'h' - Input;
}