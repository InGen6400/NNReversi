// NNRiversi.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "Board.h"


int main()
{
	char tmp[50];
	Board *mainBoard;
	mainBoard = Board_New();
	Board_Draw(mainBoard);
	Board_Delete(mainBoard);
	scanf("%d", &tmp);
    return 0;
}

