#pragma once

// Streaming SIMD Extensions Intrinsics include file for 64bit 2-packed 32bit numbers
//#include <mmintrin.h>
// Streaming SIMD Extensions Intrinsics include file for 128bit 4-packed 32bit numbers
//#include <xmmintrin.h>
// principal header file for Willamette New Instruction intrinsics
#include <emmintrin.h>

/*
#undef _mm_floor_ps

// the following code is taken from http://delibrary.googlecode.com/svn-history/r6/trunk/delight_noises/include/noisesGlobals.hpp

const _CRT_ALIGN( 16 ) int sign_mask[] = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
const _CRT_ALIGN( 16 ) float two_pow_23[] = { 8388608.f, 8388608.f, 8388608.f, 8388608.f };
const _CRT_ALIGN( 16 ) float one[] = { 1.f, 1.f, 1.f, 1.f };

// sse2 compatible rounding function
inline __m128 _mm_round_ps(__m128 x)
{
	__m128 t = _mm_or_ps(_mm_and_ps(_mm_load_ps((const float*)sign_mask), x), _mm_load_ps(two_pow_23));
	return _mm_sub_ps(_mm_add_ps(x, t), t);
}

// sse2 compatible floor function
inline __m128 _mm_floor_ps(__m128 x)
{
	__m128 t = _mm_round_ps(x);
	return _mm_sub_ps(t, _mm_and_ps(_mm_cmplt_ps(x, t), _mm_load_ps(one)));
}
*/
