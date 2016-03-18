/*
 * $Id: compiler.h,v 1.18.6.1 Broadcom SDK $
 * $Copyright: Copyright 2008 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * Compiler Specific defines and options
 */

#ifndef _SAL_COMPILER_H
#define _SAL_COMPILER_H

/*
 * Define attributes according to compiler.
 * Currently we have used GNU C, Diab Data, and Borland compilers.
 */

#define COMPILER_HAS_CONST
#define COMPILER_HAS_STATIC

#ifndef __KERNEL__
#define COMPILER_HAS_DOUBLE
#endif


/*
 * Return a string containing the current FILE:LINE location in the code. 
 *
 */
#ifndef FILE_LINE_STRING

#define __STRINGIFY(x) #x
#define _STRINGIFY(x) __STRINGIFY(x)

#define FILE_LINE_STRING() (__FILE__ ":" _STRINGIFY(__LINE__))

#endif /* FILE_LINE_STRING */


#if defined(__GNUC__) && !defined(__PEDANTIC__)

#define COMPILER_HAS_LONGLONG
#if defined(VXWORKS) && ((CPU == PPC603) || (CPU == PPC32))
/*
 * WRS T2 (Cygnus 2.7.2) PPC compiler can't handle 64-bit properly.
 * It generates bad code for shift by 32.  It also generates false
 * "variable might be used initialized" warnings for COMPILER_ZERO.
 */
#if (VX_VERSION == 55) ||  (VX_VERSION >= 62)
# define COMPILER_HAS_LONGLONG_SHIFT
#else
# undef COMPILER_HAS_LONGLONG_SHIFT
#endif
#else
# define COMPILER_HAS_LONGLONG_SHIFT
#endif
#define COMPILER_HAS_LONGLONG_ADDSUB
#define COMPILER_HAS_LONGLONG_ANDOR
#define COMPILER_HAS_LONGLONG_COMPARE

#define COMPILER_HAS_INLINE
#define COMPILER_ATTRIBUTE(_a)	__attribute__ (_a)
#define	COMPILER_REFERENCE(_a)	((void)(_a))
#define FUNCTION_NAME() (__FUNCTION__)

#else /* !defined(__GNUC__) */

#define COMPILER_ATTRIBUTE(_a)
#define	COMPILER_REFERENCE(_a)	((void)(_a))

#ifndef FUNCTION_NAME
/*
 * No portable ANSI method to accomplish this. 
 * Just return the location in the code instead. 
 */
#define FUNCTION_NAME() FILE_LINE_STRING()

#endif /* FUNCTION_NAME */

#endif /* !defined(__GNUC__) */

#ifdef __PEDANTIC__
#define COMPILER_STRING_CONST_LIMIT	509
#endif

/*
 * Compiler overrides that can be configured in Make.local
 */
#ifdef	COMPILER_OVERRIDE_NO_LONGLONG
#undef	COMPILER_HAS_LONGLONG
#undef	COMPILER_HAS_LONGLONG_SHIFT
#undef	COMPILER_HAS_LONGLONG_ADDSUB
#undef	COMPILER_HAS_LONGLONG_ANDOR
#undef	COMPILER_HAS_LONGLONG_COMPARE
#endif

#ifdef	COMPILER_OVERRIDE_NO_DOUBLE
#undef	COMPILER_HAS_DOUBLE
#endif

#ifdef	COMPILER_OVERRIDE_NO_INLINE
#undef	COMPILER_HAS_INLINE
#endif

#ifdef	COMPILER_OVERRIDE_NO_CONST
#undef	COMPILER_HAS_CONST
#endif

#ifdef	COMPILER_OVERRIDE_NO_STATIC
#undef	COMPILER_HAS_STATIC
#endif

/*
 * 64-bit word order
 */

#ifdef __BORLAND__
/* The Borland cpp does not expand correctly in the macros below...sigh */
static int u64_MSW = 1;
static int u64_LSW = 0;
#else
# ifdef BE_HOST
#  define u64_MSW	0
#  define u64_LSW	1
# else /* LE_HOST */
#  define u64_MSW	1
#  define u64_LSW	0
# endif /* LE_HOST */
#endif /* __BORLAND__ */

/*
 * 64-bit type
 */

#ifdef LONGS_ARE_64BITS

#define COMPILER_64BIT
#define	COMPILER_UINT64			unsigned long
#define COMPILER_INT64                  long
#define u64_H(v)			(((uint32 *) &(v))[u64_MSW])
#define u64_L(v)			(((uint32 *) &(v))[u64_LSW])

#else /* !LONGS_ARE_64BITS */

#ifdef COMPILER_HAS_LONGLONG

#define COMPILER_64BIT
#define	COMPILER_UINT64			unsigned long long
#define COMPILER_INT64                  long long
#define u64_H(v)			(((uint32 *) &(v))[u64_MSW])
#define u64_L(v)			(((uint32 *) &(v))[u64_LSW])

#else /* !COMPILER_HAS_LONGLONG */

#define COMPILER_UINT64			struct { unsigned int u64_w[2]; }
#define COMPILER_INT64			struct { int u64_w[2]; }
#define u64_H(v)			((v).u64_w[u64_MSW])
#define u64_L(v)			((v).u64_w[u64_LSW])

#endif /* !COMPILER_HAS_LONGLONG */
#endif /* LONGS_ARE_64BITS */

/*
 * 32-/64-bit type conversions
 */

#ifdef COMPILER_HAS_LONGLONG_SHIFT

#define COMPILER_64_TO_32_LO(dst, src)	((dst) = (uint32) (src))
#define COMPILER_64_TO_32_HI(dst, src)	((dst) = (uint32) ((src) >> 32))
#define COMPILER_64_HI(src)		((uint32) ((src) >> 32))
#define COMPILER_64_LO(src)		((uint32) (src))
#define COMPILER_64_ZERO(dst)		((dst) = 0)
#define COMPILER_64_IS_ZERO(src)	((src) == 0)
                                       

#define COMPILER_64_SET(dst, src_hi, src_lo)				\
    ((dst) = (((uint64) (src_hi)) << 32) | ((uint64) (src_lo)))

#else /* !COMPILER_HAS_LONGLONG_SHIFT */

#define COMPILER_64_TO_32_LO(dst, src)	((dst) = u64_L(src))
#define COMPILER_64_TO_32_HI(dst, src)	((dst) = u64_H(src))
#define COMPILER_64_HI(src)		u64_H(src)
#define COMPILER_64_LO(src)		u64_L(src)
#define COMPILER_64_ZERO(dst)		(u64_H(dst) = u64_L(dst) = 0)
#define COMPILER_64_IS_ZERO(src)	(u64_H(src) == 0 && u64_L(src) == 0)

#define COMPILER_64_SET(dst, src_hi, src_lo)				\
        do {								\
            u64_H(dst) = (src_hi);                              	\
            u64_L(dst) = (src_lo);                              	\
        } while (0)

#endif /* !COMPILER_HAS_LONGLONG_SHIFT */

/*
 * 64-bit addition and subtraction
 */

#ifdef COMPILER_HAS_LONGLONG_ADDSUB

#define COMPILER_64_ADD_64(dst, src)	((dst) += (src))
#define COMPILER_64_ADD_32(dst, src)	((dst) += (src))
#define COMPILER_64_SUB_64(dst, src)	((dst) -= (src))
#define COMPILER_64_SUB_32(dst, src)	((dst) -= (src))

#else /* !COMPILER_HAS_LONGLONG_ADDSUB */

#define COMPILER_64_ADD_64(dst, src)					\
        do {								\
	    uint32 __t = u64_L(dst);					\
	    u64_L(dst) += u64_L(src);					\
	    if (u64_L(dst) < __t) {					\
	        u64_H(dst) += u64_H(src) + 1;				\
	    } else {							\
	        u64_H(dst) += u64_H(src);				\
	    }								\
	} while (0)
#define COMPILER_64_ADD_32(dst, src)					\
        do {								\
	    uint32 __t = u64_L(dst);					\
	    u64_L(dst) += (src);					\
	    if (u64_L(dst) < __t) {					\
	        u64_H(dst)++;						\
	    }								\
	} while (0)
#define COMPILER_64_SUB_64(dst, src)					\
        do {								\
	    uint32 __t = u64_L(dst);					\
	    u64_L(dst) -= u64_L(src);					\
	    if (u64_L(dst) > __t) {					\
	        u64_H(dst) -= u64_H(src) + 1;				\
	    } else {							\
	        u64_H(dst) -= u64_H(src);				\
	    }								\
	} while (0)
#define COMPILER_64_SUB_32(dst, src)					\
        do {								\
	    uint32 __t = u64_L(dst);					\
	    u64_L(dst) -= (src);					\
	    if (u64_L(dst) > __t) {					\
	        u64_H(dst)--;						\
	    }								\
	} while (0)

#endif /* !COMPILER_HAS_LONGLONG_ADDSUB */

/*
 * 64-bit logical operations
 */

#ifdef COMPILER_HAS_LONGLONG_ANDOR

#define COMPILER_64_AND(dst, src)	((dst) &= (src))
#define COMPILER_64_OR(dst, src)	((dst) |= (src))
#define COMPILER_64_XOR(dst, src)	((dst) ^= (src))
#define COMPILER_64_NOT(dst)		((dst) = ~(dst))

#else /* !COMPILER_HAS_LONGLONG_ANDOR */

#define COMPILER_64_AND(dst, src)					\
	do {								\
	    u64_H(dst) &= u64_H(src);					\
	    u64_L(dst) &= u64_L(src);					\
	} while (0)
#define COMPILER_64_OR(dst, src)					\
	do {								\
	    u64_H(dst) |= u64_H(src);					\
	    u64_L(dst) |= u64_L(src);					\
	} while (0)
#define COMPILER_64_XOR(dst, src)					\
	do {								\
	    u64_H(dst) ^= u64_H(src);					\
	    u64_L(dst) ^= u64_L(src);					\
	} while (0)
#define COMPILER_64_NOT(dst)						\
	do {								\
	    u64_H(dst) = ~u64_H(dst);					\
	    u64_L(dst) = ~u64_L(dst);					\
	} while (0)

#endif /* !COMPILER_HAS_LONGLONG_ANDOR */

#define COMPILER_64_ALLONES(dst)   \
    COMPILER_64_ZERO(dst);\
    COMPILER_64_NOT(dst)

/*
 * 64-bit shift
 */

#ifdef COMPILER_HAS_LONGLONG_SHIFT

#define COMPILER_64_SHL(dst, bits)	((dst) <<= (bits))
#define COMPILER_64_SHR(dst, bits)	((dst) >>= (bits))

#define COMPILER_64_BITTEST(val, n)     \
    (!(COMPILER_64_IS_ZERO(COMPILER_64_AND(val, ((uint64) 1) << (n)))))

#else /* !COMPILER_HAS_LONGLONG_SHIFT */

#define COMPILER_64_SHL(dst, bits)					\
	do {								\
	    int __b = (bits);						\
	    if (__b >= 32) {						\
		u64_H(dst) = u64_L(dst);				\
		u64_L(dst) = 0;						\
		__b -= 32;						\
	    }								\
	    u64_H(dst) = (u64_H(dst) << __b) |				\
		(__b ? u64_L(dst) >> (32 - __b) : 0);			\
	    u64_L(dst) <<= __b;						\
	} while (0)

#define COMPILER_64_SHR(dst, bits)					\
	do {								\
	    int __b = (bits);						\
	    if (__b >= 32) {						\
		u64_L(dst) = u64_H(dst);				\
		u64_H(dst) = 0;						\
		__b -= 32;						\
	    }								\
	    u64_L(dst) = (u64_L(dst) >> __b) |				\
		(__b ? u64_H(dst) << (32 - __b) : 0);			\
	    u64_H(dst) >>= __b;						\
	} while (0)

#define COMPILER_64_BITTEST(val, n)     \
    ( (((n) < 32) && (u64_L(val) & (1 << (n)))) || \
      (((n) >= 32) && (u64_H(val) & (1 << ((n) - 32)))) )

#endif /* !COMPILER_HAS_LONGLONG_SHIFT */

/*
 * 64-bit compare operations
 */

#ifdef COMPILER_HAS_LONGLONG_COMPARE

#define COMPILER_64_EQ(src1, src2)	((src1) == (src2))
#define COMPILER_64_NE(src1, src2)	((src1) != (src2))
#define COMPILER_64_LT(src1, src2)	((src1) <  (src2))
#define COMPILER_64_LE(src1, src2)	((src1) <= (src2))
#define COMPILER_64_GT(src1, src2)	((src1) >  (src2))
#define COMPILER_64_GE(src1, src2)	((src1) >= (src2))

#else /* !COMPILER_HAS_LONGLONG_COMPARE */

#define COMPILER_64_EQ(src1, src2)	(u64_H(src1) == u64_H(src2) && \
					 u64_L(src1) == u64_L(src2))
#define COMPILER_64_NE(src1, src2)	(u64_H(src1) != u64_H(src2) || \
					 u64_L(src1) != u64_L(src2))
#define COMPILER_64_LT(src1, src2)	(u64_H(src1) < u64_H(src2) || \
					 ((u64_H(src1) == u64_H(src2) && \
					   u64_L(src1) < u64_L(src2))))
#define COMPILER_64_LE(src1, src2)	(u64_H(src1) < u64_H(src2) || \
					 ((u64_H(src1) == u64_H(src2) && \
					   u64_L(src1) <= u64_L(src2))))
#define COMPILER_64_GT(src1, src2)	(u64_H(src1) > u64_H(src2) || \
					 ((u64_H(src1) == u64_H(src2) && \
					   u64_L(src1) > u64_L(src2))))
#define COMPILER_64_GE(src1, src2)	(u64_H(src1) > u64_H(src2) || \
					 ((u64_H(src1) == u64_H(src2) && \
					   u64_L(src1) >= u64_L(src2))))

#endif /* !COMPILER_HAS_LONGLONG_COMPARE */

/* Set up a mask of width bits offset lft_shft.  No error checking */
#define COMPILER_64_MASK_CREATE(dst, width, lft_shift)                  \
    do {                                                                \
        COMPILER_64_ALLONES(dst);                                       \
        COMPILER_64_SHR((dst), (64 - (width)));                         \
        COMPILER_64_SHL((dst), (lft_shift));                            \
    } while (0)

#define COMPILER_64_DELTA(src, last, new)\
        do { \
            COMPILER_64_ZERO(src);\
            COMPILER_64_ADD_64(src, new);\
            COMPILER_64_SUB_64(src, last);\
        } while(0)

/*
 * Some macros for double support
 *
 * You can use the COMPILER_DOUBLE macro
 * if you would prefer double precision, but it is not necessary.
 * If you need more control (or you need to print :), then
 * then you should use the COMPILER_HAS_DOUBLE macro explicitly.
 *
 */
#ifdef COMPILER_HAS_DOUBLE
#define COMPILER_DOUBLE double
#define COMPILER_DOUBLE_FORMAT "f"
#define COMPILER_64_TO_DOUBLE(f, i64) \
    ((f) = COMPILER_64_HI(i64) * 4294967296.0 + COMPILER_64_LO(i64))
#define COMPILER_32_TO_DOUBLE(f, i32) \
    ((f) = (double) (i32))
#else
#define COMPILER_DOUBLE uint32
#define COMPILER_DOUBLE_FORMAT "u"
#define COMPILER_64_TO_DOUBLE(f, i64) ((f) = COMPILER_64_LO(i64))
#define COMPILER_32_TO_DOUBLE(f, i32) ((f) = (i32))
#endif

/*
 * Version of inline that is turned off for compilers that do
 * not support inline.
 */

#ifndef INLINE
# ifdef COMPILER_HAS_INLINE
#  define INLINE	inline
# else
#  define INLINE
# endif
#endif /* !INLINE */

/*
 * Version of const that is turned off for compilers that do
 * not support const.
 */

#ifndef CONST
# ifdef COMPILER_HAS_CONST
#  define CONST		const
# else
#  define CONST
# endif
#endif /* !CONST */

/*
 * Version of static that is turned off when BROADCOM_DEBUG is defined.
 * Some compilers/linkers/OSs do not put static symbols in the
 * symbol table, which can make debugging harder.
 */

#ifndef STATIC
# if	defined(COMPILER_HAS_STATIC) && !defined(BROADCOM_DEBUG)
#  define STATIC	static
# else
/* If both ROBO & ESW are supported simultaneously we have to 
 *  use "static" to avoid multiple definitions.
 */
#if defined(BCM_ROBO_SUPPORT) && defined(BCM_ESW_SUPPORT)
#  define STATIC    static
#else
#  define STATIC
# endif
# endif
#endif /* !STATIC */

#endif	/* !_SAL_COMPILER_H */
