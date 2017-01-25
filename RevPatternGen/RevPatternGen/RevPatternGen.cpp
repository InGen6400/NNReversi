// RevPatternGen.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#define POW2_8 256

typedef unsigned long long uint64;

int main()
{

	uint64 PAT[POW2_8][POW2_8][8] = {0};
	char black, white;
	char pos;

	for (black = 0; black < POW2_8; black++) {
		for (white = 0; white < POW2_8; white++) {
			//���������ꏊ�Ƀr�b�g�������Ă�����s���Ȓl�Ȃ̂�0����
			//(���ƍ��������ɂ���}�X�͑��݂��Ȃ�)
			if ((black&white) != 0) {
				for (pos = 0; pos < 8; pos++) {
					PAT[black][white][pos] = 0;
				}
			}
			else {
				for (pos = 0; pos < 8; pos++) {
					//����ꏊ���󂢂Ă��Ȃ��Ƃ����Ȃ�
					if (((black | white) & pos) != 0) {

					}
					else {
						PAT[black][white][pos] = 0;
					}
				}
			}
		}
	}

    return 0;
}

