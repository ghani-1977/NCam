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
#include <arm_neon.h>

#define __XOREQ_8_BY__

/* group */
//#define __GROUP_u8x16__
//#define __GROUP_u16x8__
//#define __GROUP_u32x4__
#define __GROUP_u64x2__

/* batch */
#define __BATCH_u8x8__
//#define __BATCH_u16x4__
//#define __BATCH_u32x2__
//#define __BATCH_u64x1__

/* span */
#define __SPAN_16__

/* GROUP */
#if defined(__GROUP_u8x16__)
	static const uint8x16_t ff0 = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	static const uint8x16_t ff1 = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	typedef uint8x16_t group;
	#define GROUP_PARALLELISM 128
	#define FF0() ff0
	#define FF1() ff1
	#define FFAND(a,b) vandq_u8(a,b)
	#define FFOR(a,b)  vorrq_u8(a,b)
	#define FFXOR(a,b) veorq_u8(a,b)
	#define FFNOT(a)   vmvnq_u8(a)
#elif defined(__GROUP_u16x8__)
	static const uint16x8_t ff0 = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	static const uint16x8_t ff1 = { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff };
	typedef uint16x8_t group;
	#define GROUP_PARALLELISM 128
	#define FF0() ff0
	#define FF1() ff1
	#define FFAND(a,b) vandq_u16(a,b)
	#define FFOR(a,b)  vorrq_u16(a,b)
	#define FFXOR(a,b) veorq_u16(a,b)
	#define FFNOT(a)   vmvnq_u16(a)
#elif defined(__GROUP_u32x4__)
	static const uint32x4_t ff0 = { 0x0, 0x0, 0x0, 0x0 };
	static const uint32x4_t ff1 = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
	typedef uint32x4_t group;
	#define GROUP_PARALLELISM 128
	#define FF0() ff0
	#define FF1() ff1
	#define FFAND(a,b) vandq_u32(a,b)
	#define FFOR(a,b)  vorrq_u32(a,b)
	#define FFXOR(a,b) veorq_u32(a,b)
	#define FFNOT(a)   vmvnq_u32(a)
#else /* (__GROUP_u64x2__) */
	static const uint64x2_t ff0 = { 0x0, 0x0 };
	static const uint64x2_t ff1 = { 0xffffffffffffffff, 0xffffffffffffffff };
	typedef uint64x2_t group;
	#define GROUP_PARALLELISM 128
	#define FF0() ff0
	#define FF1() ff1
	#define FFAND(a,b) vandq_u64(a,b)
	#define FFOR(a,b)  vorrq_u64(a,b)
	#define FFXOR(a,b) veorq_u64(a,b)
	#define FFNOT(a)   vreinterpretq_u64_u8(vmvnq_u8(vreinterpretq_u8_u64(a)))
#endif

/* BATCH */
#if defined(__BATCH_u8x8__)
	#define MEMALIGN_VAL 8
	static const uint8x8_t ff29 = { 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29 };
	static const uint8x8_t ff02 = { 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02 };
	static const uint8x8_t ff04 = { 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 };
	static const uint8x8_t ff10 = { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 };
	static const uint8x8_t ff40 = { 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40 };
	static const uint8x8_t ff80 = { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 };
	typedef uint8x8_t batch;
	#define BYTES_PER_BATCH 8
	#define B_FFN_ALL_29() ff29
	#define B_FFN_ALL_02() ff02
	#define B_FFN_ALL_04() ff04
	#define B_FFN_ALL_10() ff10
	#define B_FFN_ALL_40() ff40
	#define B_FFN_ALL_80() ff80
	#define B_FFAND(a,b)  vand_u8(a,b)
	#define B_FFOR(a,b)   vorr_u8(a,b)
	#define B_FFXOR(a,b)  veor_u8(a,b)
	#define B_FFSH8L(a,n) vshl_n_u8(a,n)
	#define B_FFSH8R(a,n) vshr_n_u8(a,n)
#elif defined(__BATCH_u16x4__)
	#define MEMALIGN_VAL 4
	static const uint16x4_t ff29 = { 0x2929, 0x2929, 0x2929, 0x2929 };
	static const uint16x4_t ff02 = { 0x0202, 0x0202, 0x0202, 0x0202 };
	static const uint16x4_t ff04 = { 0x0404, 0x0404, 0x0404, 0x0404 };
	static const uint16x4_t ff10 = { 0x1010, 0x1010, 0x1010, 0x1010 };
	static const uint16x4_t ff40 = { 0x4040, 0x4040, 0x4040, 0x4040 };
	static const uint16x4_t ff80 = { 0x8080, 0x8080, 0x8080, 0x8080 };
	typedef uint16x4_t batch;
	#define BYTES_PER_BATCH 8
	#define B_FFN_ALL_29() ff29
	#define B_FFN_ALL_02() ff02
	#define B_FFN_ALL_04() ff04
	#define B_FFN_ALL_10() ff10
	#define B_FFN_ALL_40() ff40
	#define B_FFN_ALL_80() ff80
	#define B_FFAND(a,b)  vand_u16(a,b)
	#define B_FFOR(a,b)   vorr_u16(a,b)
	#define B_FFXOR(a,b)  veor_u16(a,b)
	#define B_FFSH8L(a,n) vshl_n_u16(a,n)
	#define B_FFSH8R(a,n) vshr_n_u16(a,n)
#elif defined(__BATCH_u32x2__)
	#define MEMALIGN_VAL 2
	static const uint32x2_t ff29 = { 0x29292929, 0x29292929 };
	static const uint32x2_t ff02 = { 0x02020202, 0x02020202 };
	static const uint32x2_t ff04 = { 0x04040404, 0x04040404 };
	static const uint32x2_t ff10 = { 0x10101010, 0x10101010 };
	static const uint32x2_t ff40 = { 0x40404040, 0x40404040 };
	static const uint32x2_t ff80 = { 0x80808080, 0x80808080 };
	typedef uint32x2_t batch;
	#define BYTES_PER_BATCH 8
	#define B_FFN_ALL_29() ff29
	#define B_FFN_ALL_02() ff02
	#define B_FFN_ALL_04() ff04
	#define B_FFN_ALL_10() ff10
	#define B_FFN_ALL_40() ff40
	#define B_FFN_ALL_80() ff80
	#define B_FFAND(a,b)  vand_u32(a,b)
	#define B_FFOR(a,b)   vorr_u32(a,b)
	#define B_FFXOR(a,b)  veor_u32(a,b)
	#define B_FFSH8L(a,n) vshl_n_u32(a,n)
	#define B_FFSH8R(a,n) vshr_n_u32(a,n)
#else /* (__BATCH_u64x1__) */
	#define MEMALIGN_VAL 1
	typedef uint64x1_t batch;
	#define BYTES_PER_BATCH 8
	#define B_FFN_ALL_29() 0x2929292929292929
	#define B_FFN_ALL_02() 0x0202020202020202
	#define B_FFN_ALL_04() 0x0404040404040404
	#define B_FFN_ALL_10() 0x1010101010101010
	#define B_FFN_ALL_40() 0x4040404040404040
	#define B_FFN_ALL_80() 0x8080808080808080
	#define B_FFAND(a,b)   vand_u64(a,b)
	#define B_FFOR(a,b)    vorr_u64(a,b)
	#define B_FFXOR(a,b)   veor_u64(a,b)
	#define B_FFSH8L(a,n)  vshl_n_u64(a,n)
	#define B_FFSH8R(a,n)  vshr_n_u64(a,n)
#endif

#define M_EMPTY()

#undef XOR_8_BY
#define XOR_8_BY(d,s1,s2) vst1_u8(d, veor_u8(vld1_u8(s1),vld1_u8(s2)))
#undef XOREQ_8_BY
#define XOREQ_8_BY(d,s) XOR_8_BY(d, d, s)
#undef COPY_8_BY
#define COPY_8_BY(d,s) *(batch*)(d) = *(batch*)(s)
#undef XOR_BEST_BY
#define XOR_BEST_BY(d,s1,s2) XOR_8_BY(d,s1,s2)

/* span */
#if defined(__SPAN_16__)
	#undef BEST_SPAN
	#define BEST_SPAN            16
	#undef XOR_BEST_BY
	#if defined(__GROUP_u8x16__)
		#define XOR_BEST_BY(d,s1,s2) vst1q_u8(d, veorq_u8(vld1q_u8(s1), vld1q_u8(s2)))
	#elif defined(__GROUP_u16x8__)
		#define XOR_BEST_BY(d,s1,s2) vst1q_u16((uint16_t *)d, veorq_u16(vld1q_u16((const uint16_t *)s1), vld1q_u16((const uint16_t *)s2)))
	#elif defined(__GROUP_u32x4__)
		#define XOR_BEST_BY(d,s1,s2) vst1q_u32((uint32_t *)d, veorq_u32(vld1q_u16((const uint32_t *)s1), vld1q_u32((const uint32_t *)s2)))
	#else /* (__GROUP_u64x2__) */
		#define XOR_BEST_BY(d,s1,s2) vst1q_u64((uint64_t *)d, veorq_u64(vld1q_u64((const uint64_t *)s1), vld1q_u64((const uint64_t *)s2)))
	#endif
	#undef XOREQ_BEST_BY
	#define XOREQ_BEST_BY(d,s)   XOR_BEST_BY(d,d,s)
	#undef COPY_BEST_BY
	#define COPY_BEST_BY(d,s)    *(group*)(d) = *(group*)(s)
#else /* (__SPAN_8__) */
	#undef BEST_SPAN
	#define BEST_SPAN            8
	#undef XOR_BEST_BY
	#define XOR_BEST_BY(d,s1,s2) XOR_8_BY(d,s1,s2)
	#undef XOREQ_BEST_BY
	#define XOREQ_BEST_BY(d,s)   XOREQ_8_BY(d,s)
	#undef COPY_BEST_BY
	#define COPY_BEST_BY(d,s)    COPY_8_BY(d,s)
#endif /* end span */

#if 0 /* fftable */
#include "fftable.h"
#else
/* 64 rows of 128 bits */
inline static void FFTABLEIN(unsigned char *tab, int g, unsigned char *data) { *(((batch*)tab)+g)=*((batch*)data); }
inline static void FFTABLEOUT(unsigned char *data, unsigned char *tab, int g) { *((batch*)data)=*(((batch*)tab)+g); }
inline static void FFTABLEOUTXORNBY(int n, unsigned char *data, unsigned char *tab, int g)
{
  int j;
  for(j=0;j<n;j++) { *(data+j)^=*(tab+8*g+j); }
}
#endif /* end fftable */
