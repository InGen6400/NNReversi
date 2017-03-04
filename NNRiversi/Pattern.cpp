#include "stdafx.h"
#include "Pattern.h"
#include "BitBoard.h"
#include "bitTest.h"
#include <immintrin.h>
#include <intrin.h>

#define LEN 16

typedef unsigned short uint16;

uint64 bitGatherAVX2(uint64 in, uint64 mask);
uint64 bitGather_Normal(uint64 in, uint64 mask);
short getIndex_AVX2(const unsigned char player, const unsigned char opp);
short getIndex_Normal(const unsigned char player, const unsigned char opp);

short (*getIndex)(const unsigned char player, const unsigned char opp);

uint64(*bitGather)(uint64 in, uint64 mask);

//�֐��|�C���^��AVX2�g�p���Ɩ��g�p���̏ꍇ�ŕʂ̊֐����w��
void setAVX(char AVX2_FLAG) {
	if (AVX2_FLAG) {
		printf(">>Set AVX2 Mode\n");
		bitGather = bitGatherAVX2;
		getIndex = getIndex_AVX2;
	}
	else{
		printf("\n>>Set Normal Mode\n");
		bitGather = bitGather_Normal;
		getIndex = getIndex_Normal;
	}
}

//AVX2�ɑΉ����Ă���ꍇ
uint64 bitGatherAVX2(uint64 in, uint64 mask) {
	//AVX2
	return _pext_u64(in, mask);
}

//AVX2���Ή��̏ꍇ
uint64 bitGather_Normal(uint64 in, uint64 mask) {
	int i, count=0;
	uint64 out=0;
	for (i = 0; i < 64; mask >>= 1, in >>= 1, i++) {
		if (mask & 1) {
			out |= (in & 1) << count;
			count++;
		}
	}
	return out;
}

//YMM���W�X�^��player��opp�̃r�b�g���Z�b�g����
inline void setData_AVX2(__m256i *ret, const unsigned char player, const unsigned char opp) {

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
}

inline void setData_Normal(__m128i *ret1, __m128i *ret2, const unsigned char player, const unsigned char opp) {

	//���̕�����AVX2�ō������\�������C��PC��AVX(1)�Ȃ̂Œf�O
	*ret1 = _mm_set_epi16(player >> 7, player >> 6, player >> 5, player >> 4, player >> 3, player >> 2, player >> 1, player);
	*ret1 = _mm_and_si128(*ret1, _mm_set1_epi16(1));

	*ret2 = _mm_set_epi16(opp >> 7, opp >> 6, opp >> 5, opp >> 4, opp >> 3, opp >> 2, opp >> 1, opp);
	*ret2 = _mm_and_si128(*ret2, _mm_set1_epi16(1));

}

inline char delta_swap(char bits, char mask, char delta) {
	char x = (bits ^ (bits >> delta)) & mask;
	return bits ^ x ^ (x << delta);
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

//player, opp����C���f�b�N�X��Ԃ�
short getIndex_AVX2(const unsigned char player, const unsigned char opp)
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
	setData_AVX2(mmy, player, opp);

	//���ꂼ��̐ς̘a
	*mmz = _mm256_madd_epi16(*mmx, *mmy);
	//16bitx16bit=32bit
	*mmz = _mm256_hadd_epi32(*mmz, *mmz);
	*mmz = _mm256_hadd_epi32(*mmz, *mmz);
	//0�o�C�g�ڂ�8�o�C�g�ڂ����ꂼ��̏o�͂ɂȂ�
	return z[0] + z[8];
}

short getIndex_Normal(const unsigned char player, const unsigned char opp)
{
	alignas(16) static const uint16 pow_3[LEN/2] = { 0x1, 0x3, 0x9, 0x1b, 0x51, 0xf3, 0x2d9, 0x88b };//1,3,9,27,81,243,729,2187
	alignas(16) static const uint16 pow_3_2[LEN/2] = { 0x1*2, 0x3*2, 0x9*2, 0x1b*2, 0x51*2, 0xf3*2, 0x2d9*2, 0x88b*2 };//(1,3,9,27,81,243,729,2187)*2

	alignas(16) uint16 y1[LEN/2] = { 0 };
	alignas(16) uint16 y2[LEN/2] = { 0 };
	alignas(16) uint16 z1[LEN/2] = { 0 };
	alignas(16) uint16 z2[LEN/2] = { 0 };

	//���W�X�g���ɓo�^
	__m128i *mmy1 = (__m128i *)y1;
	__m128i *mmy2 = (__m128i *)y2;

	//�f�[�^�̐��`
	setData_Normal(mmy1, mmy2, player, opp);

	__m128i *mmx = (__m128i *)pow_3;
	__m128i *mmz = (__m128i *)z1;
	//���ꂼ��̐ς̘a
	*mmz = _mm_madd_epi16(*mmx, *mmy1);
	//16bitx16bit=32bit
	*mmz = _mm_hadd_epi32(*mmz, *mmz);
	*mmz = _mm_hadd_epi32(*mmz, *mmz);

	//
	mmx = (__m128i *)pow_3_2;
	mmz = (__m128i *)z2;

	//���ꂼ��̐ς̘a
	*mmz = _mm_madd_epi16(*mmx, *mmy2);
	//16bitx16bit=32bit
	*mmz = _mm_hadd_epi32(*mmz, *mmz);
	*mmz = _mm_hadd_epi32(*mmz, *mmz);
	//0�o�C�g�ڂ�8�o�C�g�ڂ����ꂼ��̏o�͂ɂȂ�
	return z1[0] + z2[0];
}

short getCornerIndexUL(BitBoard *bitboard, char color) {
	return getIndex(bitGather(bitboard->stone[color], 0xe0e0c00000000000),bitGather(bitboard->stone[oppColor(color)], 0xe0e0c00000000000));
}
