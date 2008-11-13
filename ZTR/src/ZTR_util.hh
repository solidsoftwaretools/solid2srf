//

#ifndef ZTR_UTIL_HH
#define ZTR_UTIL_HH

#ifdef HAVE_CONFIG_H
#include "srf_config.h"
#endif

// Temporary, until other headers are updated
#include <iosfwd>

#include <string>
#include <vector>
#include <endian.h>

// put in a global include
#ifndef FALSE
#  define FALSE false
#endif
#ifndef TRUE
#  define TRUE true
#endif
#define ABANDON
#define ADOPT

const std::string ZTR_METADATA_VALTYPE("VALTYPE");
const std::string ZTR_METADATA_NAME("NAME");


/* Platform independant byteswap code */

static inline uint16_t ZTR_ByteSwap_16_C(uint16_t val)
{
    return ((uint16_t) (
                (uint16_t) ((uint16_t) (val) >> 8) |
                (uint16_t) ((uint16_t) (val) << 8)));
}

static inline uint32_t ZTR_ByteSwap_32_C(uint32_t val)
{
    return ((uint32_t) (
                (((uint32_t) (val) & (uint32_t) 0x000000ffU) << 24) |
                (((uint32_t) (val) & (uint32_t) 0x0000ff00U) <<  8) |
                (((uint32_t) (val) & (uint32_t) 0x00ff0000U) >>  8) |
                (((uint32_t) (val) & (uint32_t) 0xff000000U) >> 24)));
}

static inline uint64_t ZTR_ByteSwap_64_C(uint64_t val)
{
    return ((uint64_t) (
                (((uint64_t) (val) & (uint64_t) 0x00000000000000ffULL) << 56) |
                (((uint64_t) (val) & (uint64_t) 0x000000000000ff00ULL) << 40) |
                (((uint64_t) (val) & (uint64_t) 0x0000000000ff0000ULL) << 24) |
                (((uint64_t) (val) & (uint64_t) 0x00000000ff000000ULL) <<  8) |
                (((uint64_t) (val) & (uint64_t) 0x000000ff00000000ULL) >>  8) |
                (((uint64_t) (val) & (uint64_t) 0x0000ff0000000000ULL) >> 24) |
                (((uint64_t) (val) & (uint64_t) 0x00ff000000000000ULL) >> 40) |
                (((uint64_t) (val) & (uint64_t) 0xff00000000000000ULL) >> 56)));
}

/* Some optimized routines for specific arches */
#if defined (__GNUC__) && (__GNUC__ >= 2) && defined (__OPTIMIZE__)
#if defined (__pentium4__)
// 32-bit intel systems -- any post 486 would do, really.
static inline uint16_t ZTR_ByteSwap_16_IA32(uint16_t val)
{
    register uint16_t v;
    if (__builtin_constant_p (val))
        v = ZTR_ByteSwap_16_C (val);
    else
        __asm__ ("rorw $8, %w0"
                 : "=r" (v)
                 : "0" (val)
                 : "cc"); 
    return v;
}

static inline uint32_t ZTR_ByteSwap_32_IA32(uint32_t val)
{
    register uint32_t v;
    if (__builtin_constant_p (val))
        v = ZTR_ByteSwap_32_C (val);
    else
        __asm__ ("bswap %0"
                 : "=r" (v)
                 : "0" (val));
    return v;
}
static inline uint64_t ZTR_ByteSwap_64_IA32(uint64_t val)
{
    union { uint64_t ll;
        uint32_t l[2]; } w,r;
    w.ll = ((uint64_t) (val));
    if (__builtin_constant_p (w.ll))
        r.ll = ZTR_ByteSwap_64_C (w.ll);
    else
    {
        r.l[0] = ZTR_ByteSwap_32 (w.l[1]);
        r.l[1] = ZTR_ByteSwap_32 (w.l[0]);
    }
    return r.ll;
}

#define ZTR_ByteSwap_16(val) (ZTR_ByteSwap_16_IA32(val))
#define ZTR_ByteSwap_32(val) (ZTR_ByteSwap_32_IA32(val))
#define ZTR_ByteSwap_64(val) (ZTR_ByteSwap_64_IA32(val))

#elif defined (__x86_64__)
static inline uint32_t ZTR_ByteSwap_32_X86_64(uint32_t val)
{
    register uint32_t v;
    if (__builtin_constant_p (val))
        v = ZTR_ByteSwap_32_C (val);
    else
        __asm__ ("bswapl %0"
                 : "=r" (v)
                 : "0" (val));
    return v;
}

static inline uint64_t ZTR_ByteSwap_64_X86_64(uint64_t val)
{
    register uint64_t v;
    if (__builtin_constant_p (val))
        v = ZTR_ByteSwap_64_C (val);
    else
        __asm__ ("bswapq %0"
                 : "=r" (v)
                 : "0" (val));
    return v;
}

#define ZTR_ByteSwap_16(val) ZTR_ByteSwap_16_C(val)
#define ZTR_ByteSwap_32(val) ZTR_ByteSwap_32_X86_64(val)
#define ZTR_ByteSwap_64(val) ZTR_ByteSwap_64_X86_64(val)
#else
// Generic Platform Independant versions 
#define ZTR_ByteSwap_16(val) ZTR_ByteSwap_16_C(val)
#define ZTR_ByteSwap_32(val) ZTR_ByteSwap_32_C(val)
#define ZTR_ByteSwap_64(val) ZTR_ByteSwap_64_C(val)
#endif
#endif


#if WORDS_BIGENDIAN == 1

#define ZTR_ByteSwapAsRequired_4(A) (A)
#define ZTR_ByteSwapAsRequired_2(A) (A)

#else

#define ZTR_ByteSwapAsRequired_2(A) ZTR_ByteSwap_16(A)
#define ZTR_ByteSwapAsRequired_4(A) ZTR_ByteSwap_32(A)

#endif

#define DELETE( A ) { delete A; A = NULL; }
#define DELETE_ARRAY( A ) { delete [] A; A = NULL; }

void ZTR_ReportError( const std::string& errMesg );

void
ZTR_ConvertStringToIntVector( const std::string& valueString,
                              std::vector<int>* valueVec );

void
ZTR_ConvertStringToFloatVector( const std::string& valueString,
                                std::vector<float>* valueVec );

void
ZTR_Dumper( const std::string& ztr );

const std::string ZTR_Header( void );

#endif
