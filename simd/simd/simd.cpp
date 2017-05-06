// simd.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include <immintrin.h>
#include <stdlib.h>

#define LEN 8

typedef unsigned long long uint64;
typedef unsigned short uint16;

void printBits(const uint16 *x, const uint16 *y, const uint16 *z) {
	char bufx[LEN][1000];
	char bufy[LEN][1000];
	char buf[LEN][1000];
	for (int i = LEN-1; i >=0; i--) {
		itoa(z[i], buf[i], 2);
		itoa(x[i], bufx[i], 2);
		itoa(y[i], bufy[i], 2);
	}
	for (int i = LEN-1; i >=0; i--) {
		printf("%s,", bufx[i]);
	}
	printf("\n");
	for (int i = LEN-1; i >=0; i--) {
		printf("%s,", bufy[i]);
	}
	printf("\n");
	for (int i = LEN-1; i >=0; i--) {
		printf("%s,", buf[i]);
	}
	printf("\n");
}

void printBit(const uint16 *x) {
	char bufx[LEN][1000];
	for (int i = LEN-1; i >=0; i--) {
		itoa(x[i], bufx[i], 2);
	}
	for (int i = LEN-1; i >= 0; i--) {
		printf("%s,", bufx[i]);
	}
	printf("\n\n");
}

void setData(__m128i *ret, const char in) {

	//������
	uint16 space[8] = { 0x8001, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100 };//A~H�Ɍ��Ԃ��J����
	uint16 mask[8]  = { 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001 };//�����ŏ��̃r�b�g�݂̂ɂ���}�X�N
	uint16 zero[8]  = { 0 };//�����̍ŉ��ʃo�C�g����͂ɏ��������Ďg��

	//�f�[�^�Z�b�g
	__m128i *mzero = (__m128i *)zero;
	__m128i *mmul = (__m128i *)space;
	__m128i *mmask = (__m128i *)mask;

	//����
	*ret = _mm_insert_epi8(*mzero, in, 0);
	
	*ret = _mm_mullo_epi32(_mm_insert_epi8(*mzero, in, 0), *mmul);//�ŉ��o�C�g�����̓o�C�g�̃f�[�^�ɃX�y�[�X���J����
	*ret = _mm_and_si128(*ret, *mmask);//�}�X�N���|���Ċe�ŉ��ʃr�b�g�ȊO0��

}

int main()
{

	alignas(16) uint16 pow_3[LEN] = { 1,1,1,1,1,1,1,1 };
	alignas(16) uint16 y[LEN] = { 0 };
	alignas(16) uint16 z[LEN];
	alignas(16) uint16 zero[LEN] = { 0 };

	__m128i *mpow = (__m128i *)pow_3;
	__m128i *my = (__m128i *)y;
	__m128i *mz = (__m128i *)z;
	__m128i *m0 = (__m128i *)zero;

	setData(my, 0x11);

	*mz = _mm_madd_epi16(*mpow, *my);
	*mz = _mm_hadd_epi32(*mz, *m0);
	*mz = _mm_hadd_epi32(*mz, *m0);
	printBits(pow_3, y, z);
	printf("%ld\n", y[0]);
	return 0;
}

