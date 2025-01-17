/* FFdecsa -- fast decsa algorithm
 *
 * Copyright (C) 2007 Dark Avenger
 *               2003-2004  fatih89r
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <xmmintrin.h>

#define MEMALIGN_VAL 16

union __u128 {
    unsigned int u[4];
    __m128 v;
};

static const union __u128 ff0 = {{0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U}};
static const union __u128 ff1 = {{0xffffffffU, 0xffffffffU, 0xffffffffU, 0xffffffffU}};

typedef __m128 group;
#define GROUP_PARALLELISM 128
#define FF0() ff0.v
#define FF1() ff1.v
#define FFAND(a,b) _mm_and_ps((a),(b))
#define FFOR(a,b)  _mm_or_ps((a),(b))
#define FFXOR(a,b) _mm_xor_ps((a),(b))
#define FFNOT(a)   _mm_xor_ps((a),FF1())
#define MALLOC(X)  _mm_malloc(X,16)
#define FREE(X)    _mm_free(X)

union __u64 {
    unsigned int u[2];
    __m64 v;
};

static const union __u64 ff29 = {{0x29292929U, 0x29292929U}};
static const union __u64 ff02 = {{0x02020202U, 0x02020202U}};
static const union __u64 ff04 = {{0x04040404U, 0x04040404U}};
static const union __u64 ff10 = {{0x10101010U, 0x10101010U}};
static const union __u64 ff40 = {{0x40404040U, 0x40404040U}};
static const union __u64 ff80 = {{0x80808080U, 0x80808080U}};

typedef __m64 batch;
#define BYTES_PER_BATCH 8
#define B_FFN_ALL_29() ff29.v
#define B_FFN_ALL_02() ff02.v
#define B_FFN_ALL_04() ff04.v
#define B_FFN_ALL_10() ff10.v
#define B_FFN_ALL_40() ff40.v
#define B_FFN_ALL_80() ff80.v
#define B_FFAND(a,b)  _mm_and_si64((a),(b))
#define B_FFOR(a,b)   _mm_or_si64((a),(b))
#define B_FFXOR(a,b)  _mm_xor_si64((a),(b))
#define B_FFSH8L(a,n) _mm_slli_si64((a),(n))
#define B_FFSH8R(a,n) _mm_srli_si64((a),(n))

#define M_EMPTY()     _mm_empty()


#undef XOR_8_BY
#define XOR_8_BY(d,s1,s2)    do { *(__m64*)d = _mm_xor_si64(*(__m64*)(s1), *(__m64*)(s2)); } while(0)

#undef XOREQ_8_BY
#define XOREQ_8_BY(d,s)      XOR_8_BY(d, d, s)

#undef COPY_8_BY
#define COPY_8_BY(d,s)       do { *(__m64 *)(d) = *(__m64 *)(s); } while(0)

#undef BEST_SPAN
#define BEST_SPAN            16

#undef XOR_BEST_BY
inline static void XOR_BEST_BY(unsigned char *d, unsigned char *s1, unsigned char *s2)
{
    __m128 vs1 = _mm_load_ps((float*)s1);
    __m128 vs2 = _mm_load_ps((float*)s2);
    vs1 = _mm_xor_ps(vs1, vs2);
    _mm_store_ps((float*)d, vs1);
}

#include "fftable.h"
