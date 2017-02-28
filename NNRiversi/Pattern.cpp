#include "stdafx.h"
#include "Pattern.h"
#include "BitBoard.h"
#include <immintrin.h>
#include <intrin.h>

#define LEN 16

typedef unsigned short uint16;

inline char delta_swap(char bits, char mask, char delta) {
	char x = (bits ^ (bits >> delta)) & mask;
	return bits ^ x ^ (x << delta);
}

//YMM���W�X�^��player��opp�̃r�b�g���Z�b�g����
inline void setData(__m256i *ret, const unsigned char player, const unsigned char opp) {

#pragma region AVX2
	//�V�t�g�p�t�B���^
	static const __m256i shifter = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
	__m256i mp = _mm256_set1_epi16(player);
	__m256i mo = _mm256_set1_epi16(opp);

	//32�r�b�g���Ƃɂ����V�t�g�ł��Ȃ��̂Ńp�b�N����
	*ret = _mm256_unpackhi_epi16(mp, mo);
	//32�r�b�g���ƂɃV�t�g
	*ret = _mm256_srlv_epi32(*ret, shifter);
	//0001�Ń}�X�N
	*ret = _mm256_and_si256(*ret, _mm256_set1_epi16(0x0001));

#pragma endregion

#pragma region AVX1
	/*
	//���̕�����AVX2�ō������\�������C��PC��AVX(1)�Ȃ̂Œf�O
	*ret = _mm256_set_epi16(me >> 7, me >> 6, me >> 5, me >> 4, me >> 3, me >> 2, me >> 1, me,
	opp >> 7, opp >> 6, opp >> 5, opp >> 4, opp >> 3, opp >> 2, opp >> 1, opp);
	*ret = _mm256_and_si256(*ret, _mm256_set1_epi16(0x0001));
	*/
#pragma endregion
}

//�r�b�g�����E�t�]�������̂�Ԃ�
inline char getMirrorLine(char in) {
	char data;
	data = ((in & 0x55) << 1) | ((in & 0xAA) >> 1);
	data = ((data & 0x33) << 2) | ((data & 0xCC) >> 2);
	return ((data & 0x0F) << 4) | ((data & 0xF0) >> 4);
}

//�Ίp���Ŏ��Ώ�
inline char getMirrorCorner(char in) {
	in = delta_swap(in, 0b00000001, 2);//6,8
	in = delta_swap(in, 0b00010000, 2);//2,4
	return delta_swap(in, 0b00000010, 4);//3,7
}

short getCornerIndexUL(BitBoard *bitboard, char color) {
	return getIndex(_pext_u64(bitboard->stone[color],0xe0e0c00000000000),_pext_u64(bitboard->stone[oppColor(color)], 0xe0e0c00000000000));
}

//player, opp����C���f�b�N�X��Ԃ�
inline short getIndex(const unsigned char player, const unsigned char opp)
{
	alignas(16) static const uint16 pow_3[LEN] = { 0x1,  0x1 * 2,  0x3,  0x3 * 2,  0x9,   0x9 * 2,   0x1b,  0x1b * 2,
		0x51, 0x51 * 2, 0xf3, 0xf3 * 2, 0x2d9, 0x2d9 * 2, 0x88b, 0x88b * 2 };//(1,3,9,27,81,243,729,2187)*2 ��1,3,9,27,81,243,729,2187
	alignas(16) uint16 y[LEN] = { 0 };
	alignas(16) uint16 z[LEN] = { 0 };

	//���W�X�g���ɓo�^
	__m256i *mmx = (__m256i *)pow_3;
	__m256i *mmy = (__m256i *)y;
	__m256i *mmz = (__m256i *)z;

	//�f�[�^�̐��`
	setData(mmy, player, opp);

	//���ꂼ��̐ς̘a
	*mmz = _mm256_madd_epi16(*mmx, *mmy);
	//16bitx16bit=32bit
	*mmz = _mm256_hadd_epi32(*mmz, *mmz);
	*mmz = _mm256_hadd_epi32(*mmz, *mmz);
	//0�o�C�g�ڂ�8�o�C�g�ڂ����ꂼ��̏o�͂ɂȂ�
	return z[0] + z[8];
}