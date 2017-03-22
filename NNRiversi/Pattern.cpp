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
	int turn, id;
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

}

void Pattern_Load() {
	FILE *fp;
	int turn, id;
	fp = fopen(PATTERN_VALUE_FILE, "rb");
	if (!fp) {
		printf("[pattern value] file OPEN error : no file?\n");
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

void Pattern_Save() {
	FILE *fp;
	int turn, id;
	fp = fopen(PATTERN_VALUE_FILE, "wb");
	if (!fp) {
		printf("[pattern value] file OPEN error\n");
		return;
	}

	printf("[pattern value] SAVE");
	for (turn = 0; turn < 16; turn++) {
		for (id = 0; id < PATTERN_AMOUNT; id++) {
			if (fwrite(PatternValue[turn][id], sizeof(int), PatternIndex[id], fp) < (size_t)PatternIndex[id]) {
				printf("\n>>>[pattern value] flie WRITE error!!<<<\n");
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

//AVX2�ɑΉ����Ă���ꍇ
inline unsigned char bitGatherAVX2(uint64 in, uint64 mask) {
	//AVX2
	return _pext_u64(in, mask);
}

//AVX2���Ή��̏ꍇ_���C���łƂ肠��������������(AVX2�œ��삷�邱�Ƃ��O��Ȃ̂ŎG�Ȃ���)
inline unsigned char bitGather_Normal(uint64 in, uint64 mask) {
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
inline unsigned short getIndex_AVX2(const unsigned char player, const unsigned char opp)
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
inline unsigned short getIndex_Normal(const unsigned char player, const unsigned char opp)
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
inline unsigned short getCornerIndexUL(uint64 black, uint64 white) {
	return getIndex(bitGather(black, 0xe0e0c00000000000), bitGather(white, 0xe0e0c00000000000));
}

//�E��̃C���f�b�N�X
inline unsigned short getCornerIndexUR(uint64 black, uint64 white) {
	return getIndex(getMirrorCorner_LR(bitGather(black, 0x0707030000000000)), getMirrorCorner_LR(bitGather(white, 0x0707030000000000)));
}

//�����̃C���f�b�N�X
inline unsigned short getCornerIndexDL(uint64 black, uint64 white) {
	return getIndex(getMirrorCorner_LR(getMirrorLine8(bitGather(black, 0x0000000000C0E0E0))), getMirrorCorner_LR(getMirrorLine8(bitGather(white, 0x0000000000C0E0E0))));
}

//�E���̃C���f�b�N�X
inline unsigned short getCornerIndexDR(uint64 black, uint64 white) {
	return getIndex(getMirrorLine8(bitGather(black, 0x0000000000030707)), getMirrorLine8(bitGather(white, 0x0000000000030707)));
}

//�����̃C���f�b�N�X���擾
inline unsigned short getLineIndex(uint64 black, uint64 white, uint64 mask) {
	return getIndex(bitGather(black, mask), bitGather(white, mask));
}

//���ォ��E�ւ̃G�b�W
#pragma region UL_R

inline unsigned short getEdgeIndexUL_R(uint64 black, uint64 white) {
	return getIndex(bitGather(black, 0xFE40000000000000), bitGather(white, 0xFE40000000000000));
}

#pragma endregion

//���ォ�牺�ւ̃G�b�W
#pragma region UL_D

inline unsigned char swap_EdgeUL_D(unsigned char in) {
	return (((in & 0b00011111) << 1) | (in & 0b11000000) | ((in & 0b00100000) >> 5));
}

inline unsigned short getEdgeIndexUL_D(uint64 black, uint64 white) {
	return getIndex(swap_EdgeUL_D(bitGather(black, 0x80C0808080808000)), swap_EdgeUL_D(bitGather(white, 0x80C0808080808000)));
}

#pragma endregion

//�E�ォ�獶�ւ̃G�b�W
#pragma region UR_L

inline unsigned char swap_EdgeUR_L(unsigned char in) {
	return (((in & 0b01111111) << 1) | ((in & 0b10000000) >> 7));
}

inline unsigned short getEdgeIndexUR_L(uint64 black, uint64 white) {
	return getIndex(swap_EdgeUR_L(getMirrorLine8(bitGather(black, 0x7F02000000000000))), swap_EdgeUR_L(getMirrorLine8(bitGather(white, 0x7F02000000000000))));
}

#pragma endregion

//�E�ォ�牺�ւ̃G�b�W
#pragma region UR_D

inline unsigned char swap_EdgeUR_D(unsigned char in) {
	return (((in & 0b00111111) << 1) | (in & 0b10000000) | ((in & 0b01000000) >> 6));
}

inline unsigned short getEdgeIndexUR_D(uint64 black, uint64 white) {
	return getIndex(swap_EdgeUR_D(bitGather(black, 0x0103010101010100)), swap_EdgeUR_D(bitGather(white, 0x0103010101010100)));
}

#pragma endregion

//�E�������ւ̃G�b�W
#pragma region DR_U

inline unsigned char swap_EdgeDR_U(unsigned char in) {
	return (((in & 0b00011111) << 1) | (in & 0b11000000) | ((in & 0b00100000) >> 5));
}

inline unsigned short getEdgeIndexDR_U(uint64 black, uint64 white) {
	return getIndex(swap_EdgeDR_U(getMirrorLine8(bitGather(black, 0x0001010101010301))), swap_EdgeDR_U(getMirrorLine8(bitGather(white, 0x0001010101010301))));
}

#pragma endregion

//�E�����獶�ւ̃G�b�W
#pragma region DR_L

inline unsigned short getEdgeIndexDR_L(uint64 black, uint64 white) {
	return getIndex(getMirrorLine8(bitGather(black, 0x000000000000027F)), getMirrorLine8(bitGather(white, 0x000000000000027F)));
}

#pragma endregion

//��������E�ւ̃G�b�W
#pragma region DL_R

inline unsigned char swap_EdgeDL_R(unsigned char in) {
	return (((in & 0b01111111) << 1) | ((in & 0b10000000) >> 7));
}

inline unsigned short getEdgeIndexDL_R(uint64 black, uint64 white) {
	return getIndex(swap_EdgeDL_R(bitGather(black, 0x00000000000040FE)), swap_EdgeDL_R(bitGather(white, 0x00000000000040FE)));
}

#pragma endregion

//���������ւ̃G�b�W
#pragma region DL_U

inline unsigned char swap_EdgeDL_U(unsigned char in) {
	return (((in & 0b00111111) << 1) | (in & 0b10000000) | ((in & 0b01000000) >> 6));
}

inline unsigned short getEdgeIndexDL_U(uint64 black, uint64 white) {
	return getIndex(swap_EdgeDL_U(getMirrorLine8(bitGather(black, 0x008080808080C080))), swap_EdgeDR_U(getMirrorLine8(bitGather(white, 0x008080808080C080))));
}

#pragma endregion

#pragma endregion

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
	ret += PatternValue[left][PATTERN_STONEDIFF][(black_count = BitBoard_CountStone(black)) - (white_count = BitBoard_CountStone(white)) + 64];//BLACK���猩��WHITE�Ƃ̐΍�
	//�p�̐΍�(0x8100000000000081�͊p�̃}�X�N)
	ret += PatternValue[left][PATTERN_STONEDIFF][BitBoard_CountStone(black & 0x8100000000000081) - BitBoard_CountStone(white & 0x8100000000000081) + 4];
	//X�̐΍� (0x0042000000004200��X�̃}�X�N)
	ret += PatternValue[left][PATTERN_STONEDIFF][BitBoard_CountStone(black & 0x0042000000004200) - BitBoard_CountStone(white & 0x0042000000004200) + 4];
	//�p���e�B�[
	ret += PatternValue[left][PATTERN_PARITY][(BITBOARD_SIZE * BITBOARD_SIZE - black_count - white_count)&1];//�󂫂܂����̋��

	return ret;
}

inline void UpdatePattern(char left, char pattern, short index, short mirror, int diff) {
	PatternValue[left][pattern][index] += diff;
	//�������]�C���f�b�N�X������Ȃ瓯���l��������
	PatternValue[left][pattern][mirror] = PatternValue[left][pattern][index];
}

inline void UpdatePatternEdge(char turn, char pattern, short index, int diff) {
	PatternValue[turn][pattern][index] += diff;
}

void UpdateAllPattern(uint64 black, uint64 white, int value, char left) {
	int index, diff;
	char blackCount, whiteCount;

	diff = (int)((value - getValue(black, white, left))*UPDATE_RATIO);

	//printf("diff:%d\n", diff);
	left /= 4;
	index = getLineIndex(black, white, 0x000000FF00000000);//y=4
	UpdatePattern(left, PATTERN_LINE4, index, getMirrorLine8(index), diff);
	index = getLineIndex(black, white, 0x00000000FF000000);//y=5
	UpdatePattern(left, PATTERN_LINE4, index, getMirrorLine8(index), diff);
	index = getLineIndex(black, white, 0x0808080808080808);//x=5
	UpdatePattern(left, PATTERN_LINE4, index, getMirrorLine8(index), diff);
	index = getLineIndex(black, white, 0x1010101010101010);//x=4
	UpdatePattern(left, PATTERN_LINE4, index, getMirrorLine8(index), diff);

	index = getLineIndex(black, white, 0x0000FF0000000000);//y=3
	UpdatePattern(left, PATTERN_LINE3, index, getMirrorLine8(index), diff);
	index = getLineIndex(black, white, 0x0000000000FF0000);//y=6
	UpdatePattern(left, PATTERN_LINE3, index, getMirrorLine8(index), diff);
	index = getLineIndex(black, white, 0x2020202020202020);//x=3
	UpdatePattern(left, PATTERN_LINE3, index, getMirrorLine8(index), diff);
	index = getLineIndex(black, white, 0x0404040404040404);//x=6
	UpdatePattern(left, PATTERN_LINE3, index, getMirrorLine8(index), diff);

	index = getLineIndex(black, white, 0x00FF000000000000);//y=2
	UpdatePattern(left, PATTERN_LINE2, index, getMirrorLine8(index), diff);
	index = getLineIndex(black, white, 0x000000000000FF00);//y=7
	UpdatePattern(left, PATTERN_LINE2, index, getMirrorLine8(index), diff);
	index = getLineIndex(black, white, 0x4040404040404040);//x=2
	UpdatePattern(left, PATTERN_LINE2, index, getMirrorLine8(index), diff);
	index = getLineIndex(black, white, 0x0202020202020202);//x=7
	UpdatePattern(left, PATTERN_LINE2, index, getMirrorLine8(index), diff);
														  //�΂ߐ�
	index = getLineIndex(black, white, 0x8040201008040201);//UL-DR
	UpdatePattern(left, PATTERN_DIAG8, index, getMirrorLine8(index), diff);
	index = getLineIndex(black, white, 0x0102040810204080);//UR-DL
	UpdatePattern(left, PATTERN_DIAG8, index, getMirrorLine8(index), diff);
	
	index = getLineIndex(black, white, 0x4020100804020100);//2,1 - 8,7
	UpdatePattern(left, PATTERN_DIAG7, index, getMirrorLine7(index), diff);
	index = getLineIndex(black, white, 0x0080402010080402);//1,2 - 7,8
	UpdatePattern(left, PATTERN_DIAG7, index, getMirrorLine7(index), diff);
	index = getLineIndex(black, white, 0x0204081020408000);//7,1 - 1,7
	UpdatePattern(left, PATTERN_DIAG7, index, getMirrorLine7(index), diff);
	index = getLineIndex(black, white, 0x0001020408102040);//8,2 - 2,8
	UpdatePattern(left, PATTERN_DIAG7, index, getMirrorLine7(index), diff);

	index = getLineIndex(black, white, 0x0000804020100804);//1,3 - 6,8
	UpdatePattern(left, PATTERN_DIAG6, index, getMirrorLine6(index), diff);
	index = getLineIndex(black, white, 0x2010080402010000);//3,1 - 8,6
	UpdatePattern(left, PATTERN_DIAG6, index, getMirrorLine6(index), diff);
	index = getLineIndex(black, white, 0x0408102040800000);//6,1 - 1,6
	UpdatePattern(left, PATTERN_DIAG6, index, getMirrorLine6(index), diff);
	index = getLineIndex(black, white, 0x0000010204081020);//8,3 - 3,8
	UpdatePattern(left, PATTERN_DIAG6, index, getMirrorLine6(index), diff);

	index = getLineIndex(black, white, 0x0000008040201008);//1,4 - 5,8
	UpdatePattern(left, PATTERN_DIAG5, index, getMirrorLine5(index), diff);
	index = getLineIndex(black, white, 0x1008040201000000);//4,1 - 8,5
	UpdatePattern(left, PATTERN_DIAG5, index, getMirrorLine5(index), diff);
	index = getLineIndex(black, white, 0x0810204080000000);//5,1 - 1,5
	UpdatePattern(left, PATTERN_DIAG5, index, getMirrorLine5(index), diff);
	index = getLineIndex(black, white, 0x0000000102040810);//8,4 - 4,8
	UpdatePattern(left, PATTERN_DIAG5, index, getMirrorLine5(index), diff);
	
	index = getLineIndex(black, white, 0x0000000080402010);//1,5 - 4,8
	UpdatePattern(left, PATTERN_DIAG4, index, getMirrorLine4(index), diff);
	index = getLineIndex(black, white, 0x0804020100000000);//5,1 - 8,4
	UpdatePattern(left, PATTERN_DIAG4, index, getMirrorLine4(index), diff);
	index = getLineIndex(black, white, 0x1020408000000000);//4,1 - 1,4
	UpdatePattern(left, PATTERN_DIAG4, index, getMirrorLine4(index), diff);
	index = getLineIndex(black, white, 0x0000000001020408);//8,5 - 5,8
	UpdatePattern(left, PATTERN_DIAG4, index, getMirrorLine4(index), diff);

	index = getCornerIndexUL(black, white);
	UpdatePattern(left, PATTERN_CORNER, index, getMirrorCorner_Diag(index), diff);
	index = getCornerIndexUR(black, white);
	UpdatePattern(left, PATTERN_CORNER, index, getMirrorCorner_Diag(index), diff);
	index = getCornerIndexDR(black, white);
	UpdatePattern(left, PATTERN_CORNER, index, getMirrorCorner_Diag(index), diff);
	index = getCornerIndexDL(black, white);
	UpdatePattern(left, PATTERN_CORNER, index, getMirrorCorner_Diag(index), diff);

	index = getEdgeIndexUL_D(black, white);
	UpdatePatternEdge(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexUL_R(black, white);
	UpdatePatternEdge(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexUR_L(black, white);
	UpdatePatternEdge(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexUR_D(black, white);
	UpdatePatternEdge(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexDR_U(black, white);
	UpdatePatternEdge(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexDR_L(black, white);
	UpdatePatternEdge(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexDL_U(black, white);
	UpdatePatternEdge(left, PATTERN_EDGE, index, diff);
	index = getEdgeIndexDL_R(black, white);
	UpdatePatternEdge(left, PATTERN_EDGE, index, diff);

	index = BitBoard_CountStone(BitBoard_getMobility(black, white));
	UpdatePatternEdge(left, PATTERN_MOBILITY_B, index, diff);
	index = BitBoard_CountStone(BitBoard_getMobility(white, black));
	UpdatePatternEdge(left, PATTERN_MOBILITY_W, index, diff);

	UpdatePatternEdge(left, PATTERN_STONEDIFF, (blackCount = BitBoard_CountStone(black)) - (whiteCount = BitBoard_CountStone(white)), diff);

	UpdatePatternEdge(left, PATTERN_PARITY, (BITBOARD_SIZE * BITBOARD_SIZE - blackCount - whiteCount) & 1, diff);
}