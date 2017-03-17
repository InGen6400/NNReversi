#pragma once
#include <intrin.h>
#include "BitBoard.h"

class M256I {
public:
	__m256i m256i;
	M256I() {};
	M256I(__m256i in) : m256i(in) {};
	M256I(uint64 in) : m256i(_mm256_set1_epi64x(in)) {};
	M256I(uint64 x, uint64 y, uint64 z, uint64 w) : m256i(_mm256_set_epi64x(x,y,z,w)) {};

	M256I operator+() { return *this; }
	M256I operator-() { return _mm256_sub_epi64(_mm256_setzero_si256(), this->m256i); }
	M256I operator~() { return _mm256_andnot_si256(m256i, M256I(0xFFFFFFFFFFFFFFFFULL).m256i); }

	M256I operator+(const int in) { return _mm256_add_epi64(this->m256i, M256I(in).m256i); }
	M256I operator+(const M256I &in) { return _mm256_add_epi64(this->m256i, in.m256i); }

	M256I operator-(const int in) { return _mm256_sub_epi64(this->m256i, M256I(in).m256i); }
	M256I operator-(const M256I &in) { return _mm256_sub_epi64(this->m256i, in.m256i); }
	
	M256I operator >> (const int shift) { return _mm256_srli_epi64(this->m256i, shift); }
	M256I operator << (const int shift) { return _mm256_slli_epi64(this->m256i, shift); }

	M256I operator|(const M256I &in) { return _mm256_or_si256(this->m256i, in.m256i); }
	M256I operator&(const M256I &in) { return _mm256_and_si256(this->m256i, in.m256i); }

};