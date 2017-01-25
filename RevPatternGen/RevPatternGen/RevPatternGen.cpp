// RevPatternGen.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"

#define POW2_8 256

typedef unsigned long long uint64;

int main()
{

	char PAT[POW2_8][POW2_8][8] = {0};
	char me, enm;
	char pos;
	char mask;
	char result = 0;
	//8�}�X�̑S�p�^�[�����[�v
	for (me = 0; me < POW2_8; me++) {
		for (enm = 0; enm < POW2_8; enm++) {
			//���������ꏊ�Ƀr�b�g�������Ă�����s���Ȓl�Ȃ̂�0����
			//(���ƍ��������ɂ���}�X�͑��݂��Ȃ�)
			if ((me&enm) != 0) {
				for (pos = 0; pos < 8; pos++) {
					PAT[me][enm][pos] = 0;
				}
			}
			else {
				//8�}�X���ׂĂɑ΂���
				for (pos = 0; pos < 8; pos++) {
					result = 0;
					//����ꏊ���󂢂Ă��Ȃ��Ƃ����Ȃ�
					if (((me | enm) & pos) != 0) {

						//�E�����ɑ΂���
						mask = (pos >> 1) & 0x7E;
						//�G�΂��A�������
						while (mask != 0 && (mask & enm) != 0) {
							result |= mask;
							mask = (mask >> 1) & 0x7E;
						}
						//�����̐΂����������璅��s��=0
						if ((mask & me) == 0) {
							result = 0;
						}
						else {
							PAT[me][enm][pos] = result;
						}

						//�������ɑ΂���
						mask = (pos << 1) & 0x7E;
						//�G�΂��A�������
						while (mask != 0 && (mask & enm) != 0) {
							result |= mask;
							mask = (mask << 1) & 0x7E;
						}
						//�����̐΂����������璅��s��=0
						if ((mask & me) == 0) {
							result = 0;
						}
						else {
							PAT[me][enm][pos] = result;
						}

					}
					else {
						PAT[me][enm][pos] = 0;
					}
				}
			}
		}
	}

    return 0;
}



