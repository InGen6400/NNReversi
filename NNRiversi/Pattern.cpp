#include "stdafx.h"
#include "Pattern.h"
#include "BitBoard.h"
#include "bitTest.h"
#include <intrin.h>

#define LEN 16

typedef unsigned short uint16;

unsigned char bitGatherAVX2(uint64 in, uint64 mask);
unsigned char bitGather_Normal(uint64 in, uint64 mask);
unsigned short getIndex_AVX2(const unsigned char player, const unsigned char opp);
unsigned short getIndex_Normal(const unsigned char player, const unsigned char opp);

unsigned short (*getIndex)(const unsigned char player, const unsigned char opp);

unsigned char(*bitGather)(uint64 in, uint64 mask);

//�֐��|�C���^��AVX2�g�p���Ɩ��g�p���̏ꍇ�ŕʂ̊֐����w��
void Pattern_setAVX(unsigned char AVX2_FLAG) {
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
unsigned char bitGatherAVX2(uint64 in, uint64 mask) {
	//AVX2
	return _pext_u64(in, mask);
}

//AVX2���Ή��̏ꍇ_���C���łƂ肠��������������(AVX2�œ��삷�邱�Ƃ��O��Ȃ̂ŎG�Ȃ���)
unsigned char bitGather_Normal(uint64 in, uint64 mask) {
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

//(AVX2�œ��삷�邱�Ƃ��O��Ȃ̂ŎG�Ȃ���)
inline void setData_Normal(__m128i *ret1, __m128i *ret2, const unsigned char player, const unsigned char opp) {

	//���̕�����AVX2�ō������\�������C��PC��AVX(1)�Ȃ̂Œf�O
	*ret1 = _mm_set_epi16(player >> 7, player >> 6, player >> 5, player >> 4, player >> 3, player >> 2, player >> 1, player);
	*ret1 = _mm_and_si128(*ret1, _mm_set1_epi16(1));

	*ret2 = _mm_set_epi16(opp >> 7, opp >> 6, opp >> 5, opp >> 4, opp >> 3, opp >> 2, opp >> 1, opp);
	*ret2 = _mm_and_si128(*ret2, _mm_set1_epi16(1));

}

//bit����
inline unsigned char delta_swap(unsigned char bits, unsigned char mask, unsigned char delta) {
	unsigned char x = (bits ^ (bits >> delta)) & mask;
	return bits ^ x ^ (x << delta);
}

#pragma region getMirror

//�r�b�g�����E�t�]�������̂�Ԃ�
inline unsigned char getMirrorLine8(unsigned char in) {
	unsigned char data;
	data = ((in & 0x55) << 1) | ((in & 0xAA) >> 1);
	data = ((data & 0x33) << 2) | ((data & 0xCC) >> 2);
	return ((data & 0x0F) << 4) | ((data & 0xF0) >> 4);
}

//�Ăяo�����ŕ�����₷���Ȃ�悤�Ɉȉ�4�֐��͂܂Ƃ߂Ă��Ȃ�
inline unsigned char getMirrorLine7(unsigned char in) {
	unsigned char data;
	data = ((in & 0x55) << 1) | ((in & 0xAA) >> 1);
	data = ((data & 0x33) << 2) | ((data & 0xCC) >> 2);
	return (((data & 0x0F) << 4) | ((data & 0xF0) >> 4)) >> 1;
}

inline unsigned char getMirrorLine6(unsigned char in) {
	unsigned char data;
	data = ((in & 0x55) << 1) | ((in & 0xAA) >> 1);
	data = ((data & 0x33) << 2) | ((data & 0xCC) >> 2);
	return (((data & 0x0F) << 4) | ((data & 0xF0) >> 4)) >> 2;
}

inline unsigned char getMirrorLine5(unsigned char in) {
	unsigned char data;
	data = ((in & 0x55) << 1) | ((in & 0xAA) >> 1);
	data = ((data & 0x33) << 2) | ((data & 0xCC) >> 2);
	return (((data & 0x0F) << 4) | ((data & 0xF0) >> 4)) >> 3;
}

inline unsigned char getMirrorLine4(unsigned char in) {
	unsigned char data;
	data = ((in & 0x55) << 1) | ((in & 0xAA) >> 1);
	data = ((data & 0x33) << 2) | ((data & 0xCC) >> 2);
	return (((data & 0x0F) << 4) | ((data & 0xF0) >> 4)) >> 4;
}

//�Ίp���Ŏ��Ώ�
inline unsigned char getMirrorCorner_Diag(unsigned char in) {
	in = delta_swap(in, 0b00000001, 2);//6,8
	in = delta_swap(in, 0b00010000, 2);//2,4
	return delta_swap(in, 0b00000010, 4);//3,7
}

//�p�̃C���f�b�N�X�����E���]
inline unsigned char getMirrorCorner_LR(unsigned char in) {
	in = delta_swap(in, 0b00100000, 2);//1,3
	in = delta_swap(in, 0b00000100, 2);//3,6
	return delta_swap(in, 0b00000001, 1);//7,8
}

#pragma endregion

//player, opp����C���f�b�N�X��Ԃ�
unsigned short getIndex_AVX2(const unsigned char player, const unsigned char opp)
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

//player, opp����C���f�b�N�X��Ԃ�(AVX2�œ��삷�邱�Ƃ��O��Ȃ̂ŎG�Ȃ���)
unsigned short getIndex_Normal(const unsigned char player, const unsigned char opp)
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

#pragma region GetIndexFunctions

//����̃C���f�b�N�X
inline unsigned short getCornerIndexUL(BitBoard *bitboard) {
	return getIndex(bitGather(bitboard->stone[BLACK], 0xe0e0c00000000000), bitGather(bitboard->stone[WHITE], 0xe0e0c00000000000));
}

//�E��̃C���f�b�N�X
inline unsigned short getCornerIndexUR(BitBoard *bitboard) {
	return getIndex(getMirrorCorner_LR(bitGather(bitboard->stone[BLACK], 0x0707030000000000)), getMirrorCorner_LR(bitGather(bitboard->stone[WHITE], 0x0707030000000000)));
}

//�����̃C���f�b�N�X
inline unsigned short getCornerIndexDL(BitBoard *bitboard) {
	return getIndex(getMirrorCorner_LR(getMirrorLine8(bitGather(bitboard->stone[BLACK], 0x0000000000C0E0E0))), getMirrorCorner_LR(getMirrorLine8(bitGather(bitboard->stone[WHITE], 0x0000000000C0E0E0))));
}

//�E���̃C���f�b�N�X
inline unsigned short getCornerIndexDR(BitBoard *bitboard) {
	return getIndex(getMirrorLine8(bitGather(bitboard->stone[BLACK], 0x0000000000030707)), getMirrorLine8(bitGather(bitboard->stone[WHITE], 0x0000000000030707)));
}

//�����̃C���f�b�N�X���擾
inline unsigned short getLineIndex(BitBoard *bitboard, uint64 mask) {
	return getIndex(bitGather(bitboard->stone[BLACK], mask), bitGather(bitboard->stone[WHITE], mask));
}

//���ォ��E�ւ̃G�b�W
#pragma region UL_R

inline unsigned short getEdgeIndexUL_R(BitBoard *bitboard) {
	return getIndex(bitGather(bitboard->stone[BLACK], 0xFE40000000000000), bitGather(bitboard->stone[WHITE], 0xFE40000000000000));
}

#pragma endregion

//���ォ�牺�ւ̃G�b�W
#pragma region UL_D

inline unsigned char swap_EdgeUL_D(unsigned char in) {
	return (((in & 0b00011111) << 1) | (in & 0b11000000) | ((in & 0b00100000) >> 5));
}

inline unsigned short getEdgeIndexUL_D(BitBoard *bitboard) {
	return getIndex(swap_EdgeUL_D(bitGather(bitboard->stone[BLACK], 0x80C0808080808000)), swap_EdgeUL_D(bitGather(bitboard->stone[WHITE], 0x80C0808080808000)));
}

#pragma endregion

//�E�ォ�獶�ւ̃G�b�W
#pragma region UR_L

inline unsigned char swap_EdgeUR_L(unsigned char in) {
	return (((in & 0b01111111) << 1) | ((in & 0b10000000) >> 7));
}

inline unsigned short getEdgeIndexUR_L(BitBoard *bitboard) {
	return getIndex(swap_EdgeUR_L(getMirrorLine8(bitGather(bitboard->stone[BLACK], 0x7F02000000000000))), swap_EdgeUR_L(getMirrorLine8(bitGather(bitboard->stone[WHITE], 0x7F02000000000000))));
}

#pragma endregion

//�E�ォ�牺�ւ̃G�b�W
#pragma region UR_D

inline unsigned char swap_EdgeUR_D(unsigned char in) {
	return (((in & 0b00111111) << 1) | (in & 0b10000000) | ((in & 0b01000000) >> 6));
}

inline unsigned short getEdgeIndexUR_D(BitBoard *bitboard) {
	return getIndex(swap_EdgeUR_D(bitGather(bitboard->stone[BLACK], 0x0103010101010100)), swap_EdgeUR_D(bitGather(bitboard->stone[WHITE], 0x0103010101010100)));
}

#pragma endregion

//�E�������ւ̃G�b�W
#pragma region DR_U

inline unsigned char swap_EdgeDR_U(unsigned char in) {
	return (((in & 0b00011111) << 1) | (in & 0b11000000) | ((in & 0b00100000) >> 5));
}

inline unsigned short getEdgeIndexDR_U(BitBoard *bitboard) {
	return getIndex(swap_EdgeDR_U(getMirrorLine8(bitGather(bitboard->stone[BLACK], 0x0001010101010301))), swap_EdgeDR_U(getMirrorLine8(bitGather(bitboard->stone[WHITE], 0x0001010101010301))));
}

#pragma endregion

//�E�����獶�ւ̃G�b�W
#pragma region DR_L

inline unsigned short getEdgeIndexDR_L(BitBoard *bitboard) {
	return getIndex(getMirrorLine8(bitGather(bitboard->stone[BLACK], 0x000000000000027F)), getMirrorLine8(bitGather(bitboard->stone[WHITE], 0x000000000000027F)));
}

#pragma endregion

//��������E�ւ̃G�b�W
#pragma region DL_R

inline unsigned char swap_EdgeDL_R(unsigned char in) {
	return (((in & 0b01111111) << 1) | ((in & 0b10000000) >> 7));
}

inline unsigned short getEdgeIndexDL_R(BitBoard *bitboard) {
	return getIndex(swap_EdgeDL_R(bitGather(bitboard->stone[BLACK], 0x00000000000040FE)), swap_EdgeDL_R(bitGather(bitboard->stone[WHITE], 0x00000000000040FE)));
}

#pragma endregion

//���������ւ̃G�b�W
#pragma region DL_U

inline unsigned char swap_EdgeDL_U(unsigned char in) {
	return (((in & 0b00111111) << 1) | (in & 0b10000000) | ((in & 0b01000000) >> 6));
}

inline unsigned short getEdgeIndexDL_U(BitBoard *bitboard) {
	return getIndex(swap_EdgeDL_U(getMirrorLine8(bitGather(bitboard->stone[BLACK], 0x008080808080C080))), swap_EdgeDR_U(getMirrorLine8(bitGather(bitboard->stone[WHITE], 0x008080808080C080))));
}

#pragma endregion

#pragma endregion

