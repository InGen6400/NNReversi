// NNRiversi.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "Board.h"
#include <stdlib.h>


int main()
{
	char tmp[50] = "";
	Board *mainBoard;
	mainBoard = Board_New();
	Board_Draw(mainBoard);
	while (1) {
		system("cls");
		Board_Draw(mainBoard);
		scanf("%d", &tmp);
		if (tmp[0] == '0')break;
	}
	Board_Delete(mainBoard);
    return 0;
}

