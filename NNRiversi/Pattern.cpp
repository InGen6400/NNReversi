
#include "stdafx.h"
#include "Pattern.h"
#include "BitBoard.h"
#include "bitTest.h"
#include <intrin.h>
#include <stdlib.h>

#define INDEX_LEN 16

typedef unsigned short uint16;

unsigned char bitGatherAVX2(uint64 in, uint64 mask);
unsigned char bitGather_Normal(uint64 in, uint64 mask);
unsigned short getIndex_AVX2(const unsigned char player, const unsigned char opp);
unsigned short getIndex_Normal(const unsigned char player, const unsigned char opp);

unsigned short (*getIndex)(const unsigned char player, const unsigned char opp);

unsigned char(*bitGather)(uint64 in, uint64 mask);

//�֐��|�C���^��AVX2�g�p���Ɩ��g�p���̏ꍇ�ŕʂ̊֐����w��
void Pattern_setAVX(unsigned char AVX2_FLAG) {
#if __AVX2__
	printf(">>Set AVX2 Mode\n");
	bitGather = bitGatherAVX2;
	getIndex = getIndex_AVX2;
#elif __AVX__
	printf(">>Set Normal Mode\n");
	bitGather = bitGather_Normal;
	getIndex = getIndex_Normal;
#endif
}

void Pattern_Init() {
	int turn, id, i, j, in, out, c;
	int corner_c[] = { POW3_2, POW3_5, POW3_0, POW3_3, POW3_6, POW3_1, POW3_4, POW3_7 };
	printf("Initialize Pattern\n");
	
	for (turn = 0; turn < 16; turn++) {
		for (id = 0; id < PATTERN_AMOUNT; id++) {
			PatternValue[turn][id] = (int*)calloc(PatternIndex[id], sizeof(int));
			if (!PatternValue[turn][id]) {
				printf("pattern init error");
				return;
			}
		}
	}

	for (i = 0; i < POW3_8; i++) {
		in = i;
		out = 0;
		c = POW3_7;
		for (j = 0; j < 8; j++) {
			out += in % 3 * c;
			in /= 3;
			c /= 3;
		}
		if (out < i) {
			MirrorLine8[i] = out;
		}
		else {
			MirrorLine8[i] = i;
		}
	}

	for (i = 0; i < POW3_7; i++) {
		in = i;
		out = 0;
		c = POW3_6;
		for (j = 0; j < 7; j++) {
			out += in % 3 * c;
			in /= 3;
			c /= 3;
		}
		if (out < i) {
			MirrorLine7[i] = out;
		}
		else {
			MirrorLine7[i] = i;
		}
	}

	for (i = 0; i < POW3_6; i++) {
		in = i;
		out = 0;
		c = POW3_5;
		for (j = 0; j < 6; j++) {
			out += in % 3 * c;
			in /= 3;
			c /= 3;
		}
		if (out < i) {
			MirrorLine6[i] = out;
		}
		else {
			MirrorLine6[i] = i;
		}
	}

	for (i = 0; i < POW3_5; i++) {
		in = i;
		out = 0;
		c = POW3_4;
		for (j = 0; j < 5; j++) {
			out += in % 3 * c;
			in /= 3;
			c /= 3;
		}
		if (out < i) {
			MirrorLine5[i] = out;
		}
		else {
			MirrorLine5[i] = i;
		}
	}

	for (i = 0; i < POW3_4; i++) {
		in = i;
		out = 0;
		c = POW3_3;
		for (j = 0; j < 4; j++) {
			out += in % 3 * c;
			in /= 3;
			c /= 3;
		}
		if (out < i) {
			MirrorLine4[i] = out;
		}
		else {
			MirrorLine4[i] = i;
		}
	}

	for (i = 0; i < POW3_8; i++) {
		in = i;
		out = 0;
		for (j = 0; j < 8; j++) {
			out += in % 3 * corner_c[j];
			in /= 3;
		}
		if (out < i) {
			MirrorCorner8[i] = out;
		}
		else {
			MirrorCorner8[i] = i;
		}
	}
}

void Pattern_Load() {
	FILE *fp;
	int turn, id;
	fp = fopen(PATTERN_VALUE_FILE, "rb");
	if (!fp) {
		printf("[pattern value] file OPEN error : no file?\n");
		printf("Create New Pattern File...\n");
		if (Pattern_Save()) {
			printf("Save success\n");
		}
		else {
			printf("Save failed\n");
			exit(EXIT_FAILURE);
		}

		return;
	}

	printf("[pattern value] LOAD");
	for (turn = 0; turn < 16; turn++) {
		for (id = 0; id < PATTERN_AMOUNT; id++) {
			if (fread(PatternValue[turn][id], sizeof(int), PatternIndex[id], fp) < (size_t)PatternIndex[id]) {
				printf("\n>>>[pattern value] flie READ error!!<<<\n");
				fclose(fp);
				return;
			}
		}
		putchar('.');
	}
	printf(">> success\n");
	fclose(fp);
	return;
}

int Pattern_Save() {
	FILE *fp;
	int turn, id;
	fp = fopen(PATTERN_VALUE_FILE, "wb");
	if (!fp) {
		printf("[pattern value] file OPEN error\n");
		return 0;
	}
	for (turn = 0; turn < 16; turn++) {
		for (id = 0; id < PATTERN_AMOUNT; id++) {
			if (fwrite(PatternValue[turn][id], sizeof(int), PatternIndex[id], fp) < (size_t)PatternIndex[id]) {
				printf("\n>>>[pattern value] flie WRITE error!!<<<\n");
				fclose(fp);
				return 0;
			}
		}
	}
	fclose(fp);
	return 1;
}

#pragma region BitFuncs

//AVX2�ɑΉ����Ă���ꍇ
inline unsigned char bitGatherAVX2(uint64 in, uint64 mask) {
	//AVX2
	return (unsigned char)_pext_u64(in, mask);
}

//AVX2���Ή��̏ꍇ_���C���łƂ肠��������������(AVX2�œ��삷�邱�Ƃ��O��Ȃ̂ŎG�Ȃ���)
inline unsigned char bitGather_Normal(uint64 in, uint64 mask) {
	int i, count = 0;
	unsigned char out = 0;
	for (i = 0; i < 64; mask >>= 1, in >>= 1, i++) {
		if ((mask & 1) == 1) {
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

//player, opp����C���f�b�N�X��Ԃ�
inline unsigned short getIndex_AVX2(const unsigned char player, const unsigned char opp)
{
	alignas(16) static const uint16 pow_3[INDEX_LEN] = { 0x1,  0x1 * 2,  0x3,  0x3 * 2,  0x9,   0x9 * 2,   0x1b,  0x1b * 2,
		0x51, 0x51 * 2, 0xf3, 0xf3 * 2, 0x2d9, 0x2d9 * 2, 0x88b, 0x88b * 2 };//(1,3,9,27,81,243,729,2187)*2 ��1,3,9,27,81,243,729,2187�����݂�
	alignas(16) uint16 y[INDEX_LEN] = { 0 };
	alignas(16) uint16 z[INDEX_LEN] = { 0 };

	//���W�X�g���ɓo�^
	__m256i *mmx = (__m256i *)pow_3;
	__m256i *mmy = (__m256i *)y;
	__m256i *mmtmp = (__m256i *)z;

	//�f�[�^�̐��`
	setData_AVX2(mmy, player, opp);

	//���ꂼ��̐ς̘a
	*mmtmp = _mm256_madd_epi16(*mmx, *mmy);
	//16bitx16bit=32bit
	*mmtmp = _mm256_hadd_epi32(*mmtmp, *mmtmp);
	*mmtmp = _mm256_hadd_epi32(*mmtmp, *mmtmp);
	//0�o�C�g�ڂ�8�o�C�g�ڂ����ꂼ��̏o�͂ɂȂ�
	return z[0] + z[8];
}

//player, opp����C���f�b�N�X��Ԃ�(AVX2�œ��삷�邱�Ƃ��O��Ȃ̂ŎG�Ȃ���)
inline unsigned short getIndex_Normal(const unsigned char player, const unsigned char opp)
{
	alignas(16) static const uint16 pow_3[INDEX_LEN / 2] = { 0x1, 0x3, 0x9, 0x1b, 0x51, 0xf3, 0x2d9, 0x88b };//1,3,9,27,81,243,729,2187
	alignas(16) static const uint16 pow_3_2[INDEX_LEN / 2] = { 0x1 * 2, 0x3 * 2, 0x9 * 2, 0x1b * 2, 0x51 * 2, 0xf3 * 2, 0x2d9 * 2, 0x88b * 2 };//(1,3,9,27,81,243,729,2187)*2

	alignas(16) uint16 y1[INDEX_LEN / 2] = { 0 };
	alignas(16) uint16 y2[INDEX_LEN / 2] = { 0 };
	alignas(16) uint16 z1[INDEX_LEN / 2] = { 0 };
	alignas(16) uint16 z2[INDEX_LEN / 2] = { 0 };

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

#pragma region getMirror_2

//�r�b�g�����E�t�]�������̂�Ԃ�
inline unsigned char MirrorLine8_2(unsigned char in) {
	unsigned char data;
	data = ((in & 0x55) << 1) | ((in & 0xAA) >> 1);
	data = ((data & 0x33) << 2) | ((data & 0xCC) >> 2);
	return ((data & 0x0F) << 4) | ((data & 0xF0) >> 4);
}

//�p�̃C���f�b�N�X�����E���]
inline unsigned char getMirrorCorner_2_LR(unsigned char in) {
	in = delta_swap(in, 0b00100000, 2);//1,3
	in = delta_swap(in, 0b00000100, 2);//3,6
	return delta_swap(in, 0b00000001, 1);//7,8
}

#pragma endregion

#pragma region getMirror_3
/*
inline unsigned short MirrorLine8_3(unsigned char black, unsigned char white) {

	black = ((black & 0x55) << 1) | ((black & 0xAA) >> 1);
	black = ((black & 0x33) << 2) | ((black & 0xCC) >> 2);
	black = ((black & 0x0F) << 4) | ((black & 0xF0) >> 4);

	white = ((white & 0x55) << 1) | ((white & 0xAA) >> 1);
	white = ((white & 0x33) << 2) | ((white & 0xCC) >> 2);
	white = ((white & 0x0F) << 4) | ((white & 0xF0) >> 4);

	return getIndex(black, white);
}


//�Ăяo�����ŕ�����₷���Ȃ�悤�Ɉȉ�4�֐��͂܂Ƃ߂Ă��Ȃ�
inline unsigned short MirrorLine7_3(unsigned char black, unsigned char white) {

	black = ((black & 0x55) << 1) | ((black & 0xAA) >> 1);
	black = ((black & 0x33) << 2) | ((black & 0xCC) >> 2);
	black = (((black & 0x0F) << 4) | ((black & 0xF0) >> 4)) >> 1;

	white = ((white & 0x55) << 1) | ((white & 0xAA) >> 1);
	white = ((white & 0x33) << 2) | ((white & 0xCC) >> 2);
	white = (((white & 0x0F) << 4) | ((white & 0xF0) >> 4)) >> 1;

	return getIndex(black, white);
}

inline unsigned short MirrorLine6_3(unsigned char black, unsigned char white) {

	black = ((black & 0x55) << 1) | ((black & 0xAA) >> 1);
	black = ((black & 0x33) << 2) | ((black & 0xCC) >> 2);
	black = (((black & 0x0F) << 4) | ((black & 0xF0) >> 4)) >> 2;

	white = ((white & 0x55) << 1) | ((white & 0xAA) >> 1);
	white = ((white & 0x33) << 2) | ((white & 0xCC) >> 2);
	white = (((white & 0x0F) << 4) | ((white & 0xF0) >> 4)) >> 2;

	return getIndex(black, white);
}

inline unsigned short MirrorLine5_3(unsigned char black, unsigned char white) {

	black = ((black & 0x55) << 1) | ((black & 0xAA) >> 1);
	black = ((black & 0x33) << 2) | ((black & 0xCC) >> 2);
	black = (((black & 0x0F) << 4) | ((black & 0xF0) >> 4)) >> 3;

	white = ((white & 0x55) << 1) | ((white & 0xAA) >> 1);
	white = ((white & 0x33) << 2) | ((white & 0xCC) >> 2);
	white = (((white & 0x0F) << 4) | ((white & 0xF0) >> 4)) >> 3;

	return getIndex(black, white);
}

inline unsigned short MirrorLine4_3(unsigned char black, unsigned char white) {

	black = ((black & 0x55) << 1) | ((black & 0xAA) >> 1);
	black = ((black & 0x33) << 2) | ((black & 0xCC) >> 2);
	black = (((black & 0x0F) << 4) | ((black & 0xF0) >> 4)) >> 4;

	white = ((white & 0x55) << 1) | ((white & 0xAA) >> 1);
	white = ((white & 0x33) << 2) | ((white & 0xCC) >> 2);
	white = (((white & 0x0F) << 4) | ((white & 0xF0) >> 4)) >> 4;

	return getIndex(black, white);
}

//�Ίp���Ŏ��Ώ�
inline unsigned short MirrorCorner(unsigned char black, unsigned char white) {

	black = delta_swap(black, 0b00000001, 2);//6,8
	black = delta_swap(black, 0b00010000, 2);//2,4
	black = delta_swap(black, 0b00000010, 4);//3,

	white = delta_swap(white, 0b00000001, 2);//6,8
	white = delta_swap(white, 0b00010000, 2);//2,4
	white = delta_swap(white, 0b00000010, 4);//3,7

	return getIndex(black, white);
}

inline unsigned short getMirrorCorner_3_LR(unsigned char black, unsigned char white) {
	black = delta_swap(black, 0b00100000, 2);//1,3
	black = delta_swap(black, 0b00000100, 2);//3,6
	black = delta_swap(black, 0b00000001, 1);//7,8

	white = delta_swap(white, 0b00100000, 2);//1,3
	white = delta_swap(white, 0b00000100, 2);//3,6
	white = delta_swap(white, 0b00000001, 1);//7,8

	return getIndex(black, white);
}
*/
#pragma endregion

#pragma region GetIndexFunctions

//�����̃C���f�b�N�X���擾
inline unsigned short getLineIndex(uint64 black, uint64 white, uint64 mask) {
	return getIndex(bitGather(black, mask), bitGather(white, mask));
}

//����̃C���f�b�N�X
inline unsigned short getCornerIndexUL(uint64 black, uint64 white) {
	return getIndex(bitGather(black, 0xe0e0c00000000000), bitGather(white, 0xe0e0c00000000000));
}

//�E��̃C���f�b�N�X
inline unsigned short getCornerIndexUR(uint64 black, uint64 white) {
	return getIndex(getMirrorCorner_2_LR(bitGather(black, 0x0707030000000000)), getMirrorCorner_2_LR(bitGather(white, 0x0707030000000000)));
}

//�����̃C���f�b�N�X
inline unsigned short getCornerIndexDL(uint64 black, uint64 white) {
	return getIndex(getMirrorCorner_2_LR(MirrorLine8_2(bitGather(black, 0x0000000000C0E0E0))), getMirrorCorner_2_LR(MirrorLine8_2(bitGather(white, 0x0000000000C0E0E0))));
}

//�E���̃C���f�b�N�X
inline unsigned short getCornerIndexDR(uint64 black, uint64 white) {
	return getIndex(MirrorLine8_2(bitGather(black, 0x0000000000030707)), MirrorLine8_2(bitGather(white, 0x0000000000030707)));
}


//���ォ��E�ւ̃G�b�W

inline unsigned short getEdgeIndexUL_R(uint64 black, uint64 white) {
	return getIndex(bitGather(black, 0xFE40000000000000), bitGather(white, 0xFE40000000000000));
}


//���ォ�牺�ւ̃G�b�W

inline unsigned char swap_EdgeUL_D(unsigned char in) {
	return (((in & 0b00011111) << 1) | (in & 0b11000000) | ((in & 0b00100000) >> 5));
}

inline unsigned short getEdgeIndexUL_D(uint64 black, uint64 white) {
	return getIndex(swap_EdgeUL_D(bitGather(black, 0x80C0808080808000)), swap_EdgeUL_D(bitGather(white, 0x80C0808080808000)));
}


//�E�ォ�獶�ւ̃G�b�W

inline unsigned char swap_EdgeUR_L(unsigned char in) {
	return (((in & 0b01111111) << 1) | ((in & 0b10000000) >> 7));
}

inline unsigned short getEdgeIndexUR_L(uint64 black, uint64 white) {
	return getIndex(swap_EdgeUR_L(MirrorLine8_2(bitGather(black, 0x7F02000000000000))), swap_EdgeUR_L(MirrorLine8_2(bitGather(white, 0x7F02000000000000))));
}


//�E�ォ�牺�ւ̃G�b�W

inline unsigned char swap_EdgeUR_D(unsigned char in) {
	return (((in & 0b00111111) << 1) | (in & 0b10000000) | ((in & 0b01000000) >> 6));
}

inline unsigned short getEdgeIndexUR_D(uint64 black, uint64 white) {
	return getIndex(swap_EdgeUR_D(bitGather(black, 0x0103010101010100)), swap_EdgeUR_D(bitGather(white, 0x0103010101010100)));
}


//�E�������ւ̃G�b�W

inline unsigned char swap_EdgeDR_U(unsigned char in) {
	return (((in & 0b00011111) << 1) | (in & 0b11000000) | ((in & 0b00100000) >> 5));
}

inline unsigned short getEdgeIndexDR_U(uint64 black, uint64 white) {
	return getIndex(swap_EdgeDR_U(MirrorLine8_2(bitGather(black, 0x0001010101010301))), swap_EdgeDR_U(MirrorLine8_2(bitGather(white, 0x0001010101010301))));
}


//�E�����獶�ւ̃G�b�W

inline unsigned short getEdgeIndexDR_L(uint64 black, uint64 white) {
	return getIndex(MirrorLine8_2(bitGather(black, 0x000000000000027F)), MirrorLine8_2(bitGather(white, 0x000000000000027F)));
}

//��������E�ւ̃G�b�W

inline unsigned char swap_EdgeDL_R(unsigned char in) {
	return (((in & 0b01111111) << 1) | ((in & 0b10000000) >> 7));
}

inline unsigned short getEdgeIndexDL_R(uint64 black, uint64 white) {
	return getIndex(swap_EdgeDL_R(bitGather(black, 0x00000000000040FE)), swap_EdgeDL_R(bitGather(white, 0x00000000000040FE)));
}


//���������ւ̃G�b�W

inline unsigned char swap_EdgeDL_U(unsigned char in) {
	return (((in & 0b00111111) << 1) | (in & 0b10000000) | ((in & 0b01000000) >> 6));
}

inline unsigned short getEdgeIndexDL_U(uint64 black, uint64 white) {
	return getIndex(swap_EdgeDL_U(MirrorLine8_2(bitGather(black, 0x008080808080C080))), swap_EdgeDR_U(MirrorLine8_2(bitGather(white, 0x008080808080C080))));
}


#pragma endregion
#pragma endregion

inline void getBWbitsM(uint64 black, uint64 white, uint64 mask, unsigned char *retB, unsigned char *retW) {
	*retB = bitGather(black, mask);
	*retW = bitGather(white, mask);
}

inline void getBWbits_UL(uint64 black, uint64 white, unsigned char *retB, unsigned char *retW) {
	*retB = bitGather(black, 0xe0e0c00000000000);
	*retW = bitGather(white, 0xe0e0c00000000000);
}

inline void getBWbits_UR(uint64 black, uint64 white, unsigned char *retB, unsigned char *retW) {
	*retB = getMirrorCorner_2_LR(bitGather(black, 0x0707030000000000));
	*retW = getMirrorCorner_2_LR(bitGather(white, 0x0707030000000000));
}

inline void getBWbits_DL(uint64 black, uint64 white, unsigned char *retB, unsigned char *retW) {
	*retB = getMirrorCorner_2_LR(MirrorLine8_2(bitGather(black, 0x0000000000C0E0E0)));
	*retW = getMirrorCorner_2_LR(MirrorLine8_2(bitGather(white, 0x0000000000C0E0E0)));
}

inline void getBWbits_DR(uint64 black, uint64 white, unsigned char *retB, unsigned char *retW) {
	*retB = MirrorLine8_2(bitGather(black, 0x0000000000030707));
	*retW = MirrorLine8_2(bitGather(white, 0x0000000000030707));
}

int getValue(uint64 black, uint64 white, char left) {
	int ret = 0;
	char black_count, white_count;
	left /= 4;
	//����
	ret += PatternValue[left][PATTERN_LINE4][getLineIndex(black, white, 0x000000FF00000000)];//y=4
	ret += PatternValue[left][PATTERN_LINE4][getLineIndex(black, white, 0x00000000FF000000)];//y=5
	ret += PatternValue[left][PATTERN_LINE4][getLineIndex(black, white, 0x0808080808080808)];//x=5
	ret += PatternValue[left][PATTERN_LINE4][getLineIndex(black, white, 0x1010101010101010)];//x=4

	ret += PatternValue[left][PATTERN_LINE3][getLineIndex(black, white, 0x0000FF0000000000)];//y=3
	ret += PatternValue[left][PATTERN_LINE3][getLineIndex(black, white, 0x0000000000FF0000)];//y=6
	ret += PatternValue[left][PATTERN_LINE3][getLineIndex(black, white, 0x2020202020202020)];//x=3
	ret += PatternValue[left][PATTERN_LINE3][getLineIndex(black, white, 0x0404040404040404)];//x=6

	ret += PatternValue[left][PATTERN_LINE2][getLineIndex(black, white, 0x00FF000000000000)];//y=2
	ret += PatternValue[left][PATTERN_LINE2][getLineIndex(black, white, 0x000000000000FF00)];//y=7
	ret += PatternValue[left][PATTERN_LINE2][getLineIndex(black, white, 0x4040404040404040)];//x=2
	ret += PatternValue[left][PATTERN_LINE2][getLineIndex(black, white, 0x0202020202020202)];//x=7
	//�΂ߐ�
	ret += PatternValue[left][PATTERN_DIAG8][getLineIndex(black, white, 0x8040201008040201)];//UL-DR
	ret += PatternValue[left][PATTERN_DIAG8][getLineIndex(black, white, 0x0102040810204080)];//UR-DL

	ret += PatternValue[left][PATTERN_DIAG7][getLineIndex(black, white, 0x4020100804020100)];//2,1 - 8,7
	ret += PatternValue[left][PATTERN_DIAG7][getLineIndex(black, white, 0x0080402010080402)];//1,2 - 7,8
	ret += PatternValue[left][PATTERN_DIAG7][getLineIndex(black, white, 0x0204081020408000)];//7,1 - 1,7
	ret += PatternValue[left][PATTERN_DIAG7][getLineIndex(black, white, 0x0001020408102040)];//8,2 - 2,8

	ret += PatternValue[left][PATTERN_DIAG6][getLineIndex(black, white, 0x0000804020100804)];//1,3 - 6,8
	ret += PatternValue[left][PATTERN_DIAG6][getLineIndex(black, white, 0x2010080402010000)];//3,1 - 8,6
	ret += PatternValue[left][PATTERN_DIAG6][getLineIndex(black, white, 0x0408102040800000)];//6,1 - 1,6
	ret += PatternValue[left][PATTERN_DIAG6][getLineIndex(black, white, 0x0000010204081020)];//8,3 - 3,8

	ret += PatternValue[left][PATTERN_DIAG5][getLineIndex(black, white, 0x0000008040201008)];//1,4 - 5,8
	ret += PatternValue[left][PATTERN_DIAG5][getLineIndex(black, white, 0x1008040201000000)];//4,1 - 8,5
	ret += PatternValue[left][PATTERN_DIAG5][getLineIndex(black, white, 0x0810204080000000)];//5,1 - 1,5
	ret += PatternValue[left][PATTERN_DIAG5][getLineIndex(black, white, 0x0000000102040810)];//8,4 - 4,8

	ret += PatternValue[left][PATTERN_DIAG4][getLineIndex(black, white, 0x0000000080402010)];//1,5 - 4,8
	ret += PatternValue[left][PATTERN_DIAG4][getLineIndex(black, white, 0x0804020100000000)];//5,1 - 8,4
	ret += PatternValue[left][PATTERN_DIAG4][getLineIndex(black, white, 0x1020408000000000)];//4,1 - 1,4
	ret += PatternValue[left][PATTERN_DIAG4][getLineIndex(black, white, 0x0000000001020408)];//8,5 - 5,8
	//�p
	ret += PatternValue[left][PATTERN_CORNER][getCornerIndexUL(black, white)];//����
	ret += PatternValue[left][PATTERN_CORNER][getCornerIndexUR(black, white)];//�E��
	ret += PatternValue[left][PATTERN_CORNER][getCornerIndexDR(black, white)];//�E��
	ret += PatternValue[left][PATTERN_CORNER][getCornerIndexDL(black, white)];//����
	//�G�b�W

	ret += PatternValue[left][PATTERN_EDGE][getEdgeIndexUL_R(black, white)];//���ォ��E
	ret += PatternValue[left][PATTERN_EDGE][getEdgeIndexUL_D(black, white)];//���ォ�牺
	ret += PatternValue[left][PATTERN_EDGE][getEdgeIndexUR_L(black, white)];//�E�ォ�獶
	ret += PatternValue[left][PATTERN_EDGE][getEdgeIndexUR_D(black, white)];//�E�ォ�牺
	ret += PatternValue[left][PATTERN_EDGE][getEdgeIndexDR_U(black, white)];//�E�������
	ret += PatternValue[left][PATTERN_EDGE][getEdgeIndexDR_L(black, white)];//�E�����獶
	ret += PatternValue[left][PATTERN_EDGE][getEdgeIndexDL_U(black, white)];//���������
	ret += PatternValue[left][PATTERN_EDGE][getEdgeIndexDL_R(black, white)];//��������E
	//����\��
	ret += PatternValue[left][PATTERN_MOBILITY_B][BitBoard_CountStone(BitBoard_getMobility(black, white))];//BLACK�̒���\�ʒu�̐�
	ret += PatternValue[left][PATTERN_MOBILITY_W][BitBoard_CountStone(BitBoard_getMobility(white, black))];//WHITE�̒���\�ʒu�̐�
	//�΍�
	black_count = BitBoard_CountStone(black);
	white_count = BitBoard_CountStone(white);
	//ret += PatternValue[left][PATTERN_STONEDIFF][black_count - white_count + 64];//BLACK���猩��WHITE�Ƃ̐΍�
	//�p�̐΍�(0x8100000000000081�͊p�̃}�X�N)
	ret += PatternValue[left][PATTERN_CORNER_STONE][BitBoard_CountStone(black & 0x8100000000000081) - BitBoard_CountStone(white & 0x8100000000000081) + 4];
	//X�̐΍� (0x0042000000004200��X�̃}�X�N)
	//ret += PatternValue[left][PATTERN_X_STONE][BitBoard_CountStone(black & 0x0042000000004200) - BitBoard_CountStone(white & 0x0042000000004200) + 4];
	//�p���e�B�[
	//ret += PatternValue[left][PATTERN_PARITY][(BITBOARD_SIZE * BITBOARD_SIZE - black_count - white_count)&1];//�󂫂܂����̋��

	return ret;
}

inline void UpdatePattern_Mirror(char left, char pattern, unsigned short index, unsigned short mirror, int diff) {
	if ((long)PatternValue[left][pattern][index] + diff >= MAX_VALUE) {
		//�����]���l��MAX���傫��������
		PatternValue[left][pattern][index] = MAX_VALUE;
	}
	else if ((long)PatternValue[left][pattern][index] + diff <= -MAX_VALUE) {
		//�����]���l��-MAX��菬����������
		PatternValue[left][pattern][index] = -MAX_VALUE;
	}
	else {
		PatternValue[left][pattern][index] += diff;
	}
	//�������]�C���f�b�N�X������Ȃ瓯���l��������
	PatternValue[left][pattern][mirror] = PatternValue[left][pattern][index];
}

inline void UpdatePattern_nonMirror(char left, char pattern, unsigned short index, int diff) {
	if (PatternValue[left][pattern][index] + diff >= MAX_VALUE) {
		//�����]���l��MAX���傫��������
		PatternValue[left][pattern][index] = MAX_VALUE;
	}
	else if (PatternValue[left][pattern][index] + diff <= -MAX_VALUE) {
		//�����]���l��-MAX��菬����������
		PatternValue[left][pattern][index] = -MAX_VALUE;
	}
	else {
		PatternValue[left][pattern][index] += diff;
	}
}

void UpdateAllPattern(uint64 black, uint64 white, int value, char left) {
	int diff;
	unsigned short index;
	unsigned char blackCount, whiteCount;
	unsigned char bBlack, bWhite;

	diff = (int)((value - getValue(black, white, left))*UPDATE_RATIO);
	left /= 4;
	index = getLineIndex(black, white, 0x000000FF00000000);
	UpdatePattern_Mirror(left, PATTERN_LINE4,index, MirrorLine8[index], diff);
	index = getLineIndex(black, white, 0x00000000FF000000);//y=5
	UpdatePattern_Mirror(left, PATTERN_LINE4,index, MirrorLine8[index], diff);
	index = getLineIndex(black, white, 0x0808080808080808);//x=5
	UpdatePattern_Mirror(left, PATTERN_LINE4,index, MirrorLine8[index], diff);
	index = getLineIndex(black, white, 0x1010101010101010);//x=4
	UpdatePattern_Mirror(left, PATTERN_LINE4,index, MirrorLine8[index], diff);

	index = getLineIndex(black, white, 0x0000FF0000000000);//y=3
	UpdatePattern_Mirror(left, PATTERN_LINE3,index, MirrorLine8[index], diff);
	index = getLineIndex(black, white, 0x0000000000FF0000);//y=6
	UpdatePattern_Mirror(left, PATTERN_LINE3,index, MirrorLine8[index], diff);
	index = getLineIndex(black, white, 0x2020202020202020);//x=3
	UpdatePattern_Mirror(left, PATTERN_LINE3,index, MirrorLine8[index], diff);
	index = getLineIndex(black, white, 0x0404040404040404);//x=6
	UpdatePattern_Mirror(left, PATTERN_LINE3,index, MirrorLine8[index], diff);

	index = getLineIndex(black, white, 0x00FF000000000000);//y=2
	UpdatePattern_Mirror(left, PATTERN_LINE2,index, MirrorLine8[index], diff);
	index = getLineIndex(black, white, 0x000000000000FF00);//y=7
	UpdatePattern_Mirror(left, PATTERN_LINE2,index, MirrorLine8[index], diff);
	index = getLineIndex(black, white, 0x4040404040404040);//x=2
	UpdatePattern_Mirror(left, PATTERN_LINE2,index, MirrorLine8[index], diff);
	index = getLineIndex(black, white, 0x0202020202020202);//x=7
	UpdatePattern_Mirror(left, PATTERN_LINE2,index, MirrorLine8[index], diff);

														  //�΂ߐ�
	index = getLineIndex(black, white, 0x8040201008040201);//UL-DR
	UpdatePattern_Mirror(left, PATTERN_DIAG8, index, MirrorLine8[index], diff);
	index = getLineIndex(black, white, 0x0102040810204080);//UR-DL
	UpdatePattern_Mirror(left, PATTERN_DIAG8, index, MirrorLine8[index], diff);
	
	index = getLineIndex(black, white, 0x4020100804020100);//2,1 - 8,7
	UpdatePattern_Mirror(left, PATTERN_DIAG7, index, MirrorLine7[index], diff);
	index = getLineIndex(black, white, 0x0080402010080402);//1,2 - 7,8
	UpdatePattern_Mirror(left, PATTERN_DIAG7, index, MirrorLine7[index], diff);
	index = getLineIndex(black, white, 0x0204081020408000);//7,1 - 1,7
	UpdatePattern_Mirror(left, PATTERN_DIAG7, index, MirrorLine7[index], diff);
	index = getLineIndex(black, white, 0x0001020408102040);//8,2 - 2,8
	UpdatePattern_Mirror(left, PATTERN_DIAG7, index, MirrorLine7[index], diff);

	index = getLineIndex(black, white, 0x0000804020100804);//1,3 - 6,8
	UpdatePattern_Mirror(left, PATTERN_DIAG6, index, MirrorLine6[index], diff);
	index = getLineIndex(black, white, 0x2010080402010000);//3,1 - 8,6
	UpdatePattern_Mirror(left, PATTERN_DIAG6, index, MirrorLine6[index], diff);
	index = getLineIndex(black, white, 0x0408102040800000);//6,1 - 1,6
	UpdatePattern_Mirror(left, PATTERN_DIAG6, index, MirrorLine6[index], diff);
	index = getLineIndex(black, white, 0x0000010204081020);//8,3 - 3,8
	UpdatePattern_Mirror(left, PATTERN_DIAG6, index, MirrorLine6[index], diff);

	index = getLineIndex(black, white, 0x0000008040201008);//1,4 - 5,8
	UpdatePattern_Mirror(left, PATTERN_DIAG5, index, MirrorLine5[index], diff);
	index = getLineIndex(black, white, 0x1008040201000000);//4,1 - 8,5
	UpdatePattern_Mirror(left, PATTERN_DIAG5, index, MirrorLine5[index], diff);
	index = getLineIndex(black, white, 0x0810204080000000);//5,1 - 1,5
	UpdatePattern_Mirror(left, PATTERN_DIAG5, index, MirrorLine5[index], diff);
	index = getLineIndex(black, white, 0x0000000102040810);//8,4 - 4,8
	UpdatePattern_Mirror(left, PATTERN_DIAG5, index, MirrorLine5[index], diff);
	
	index = getLineIndex(black, white, 0x0000000080402010);//1,5 - 4,8
	UpdatePattern_Mirror(left, PATTERN_DIAG4, index, MirrorLine4[index], diff);
	index = getLineIndex(black, white, 0x0804020100000000);//5,1 - 8,4
	UpdatePattern_Mirror(left, PATTERN_DIAG4, index, MirrorLine4[index], diff);
	index = getLineIndex(black, white, 0x1020408000000000);//4,1 - 1,4
	UpdatePattern_Mirror(left, PATTERN_DIAG4, index, MirrorLine4[index], diff);
	index = getLineIndex(black, white, 0x0000000001020408);//8,5 - 5,8
	UpdatePattern_Mirror(left, PATTERN_DIAG4, index, MirrorLine4[index], diff);

	index = getCornerIndexUL(black, white);
	UpdatePattern_Mirror(left, PATTERN_CORNER, index, MirrorCorner8[index], diff);
	index = getCornerIndexUR(black, white);
	UpdatePattern_Mirror(left, PATTERN_CORNER, index, MirrorCorner8[index], diff);
	index = getCornerIndexDR(black, white);
	UpdatePattern_Mirror(left, PATTERN_CORNER, index, MirrorCorner8[index], diff);
	index = getCornerIndexDL(black, white);
	UpdatePattern_Mirror(left, PATTERN_CORNER, index, MirrorCorner8[index], diff);


	//printf("asd\n");
	index = getEdgeIndexUL_D(black, white);
	UpdatePattern_nonMirror(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexUL_R(black, white);
	UpdatePattern_nonMirror(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexUR_L(black, white);
	UpdatePattern_nonMirror(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexUR_D(black, white);
	UpdatePattern_nonMirror(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexDR_U(black, white);
	UpdatePattern_nonMirror(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexDR_L(black, white);
	UpdatePattern_nonMirror(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexDL_U(black, white);
	UpdatePattern_nonMirror(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexDL_R(black, white);
	UpdatePattern_nonMirror(left, PATTERN_EDGE, index, diff);

	index = BitBoard_CountStone(BitBoard_getMobility(black, white));
	UpdatePattern_nonMirror(left, PATTERN_MOBILITY_B, index, diff);
	index = BitBoard_CountStone(BitBoard_getMobility(white, black));
	UpdatePattern_nonMirror(left, PATTERN_MOBILITY_W, index, diff);

	blackCount = BitBoard_CountStone(black);
	whiteCount = BitBoard_CountStone(white);
	UpdatePattern_nonMirror(left, PATTERN_STONEDIFF, blackCount - whiteCount + 64, diff);

	UpdatePattern_nonMirror(left, PATTERN_CORNER_STONE, BitBoard_CountStone(black & 0x8100000000000081) - BitBoard_CountStone(white & 0x8100000000000081) + 4, diff);

	UpdatePattern_nonMirror(left, PATTERN_X_STONE, BitBoard_CountStone(black & 0x0042000000004200) - BitBoard_CountStone(white & 0x0042000000004200) + 4, diff);

	UpdatePattern_nonMirror(left, PATTERN_PARITY, (BITBOARD_SIZE * BITBOARD_SIZE - blackCount - whiteCount) & 1, diff);
}

void Patttern_Debug(int index) {
	printf("%d\n", index);
	printf("%d\n", MirrorCorner8[index]);
}