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
#include <byteswap.h>

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
#define ZTR_ByteSwap_16(val) ZTR_ByteSwap_16_C(val)
#define ZTR_ByteSwap_32(val) ZTR_ByteSwap_32_C(val)
#define ZTR_ByteSwap_64(val) ZTR_ByteSwap_64_C(val)

#if (WORDS_BIGENDIAN == 1) || (BYTE_ORDER == BIG_ENDIAN)
#  define ZTR_ByteSwapAsRequired_4(A) (A)
#  define ZTR_ByteSwapAsRequired_2(A) (A)
#else
#  if defined (__GNUC__) && (__GNUC__ >= 2) && defined (__OPTIMIZE__)
// #warning Using GCC optimized byteswap
#    define ZTR_ByteSwapAsRequired_2(A) bswap_16(A)
#    define ZTR_ByteSwapAsRequired_4(A) bswap_32(A)
#  else 
#    define ZTR_ByteSwapAsRequired_2(A) ZTR_ByteSwap_16(A)
#    define ZTR_ByteSwapAsRequired_4(A) ZTR_ByteSwap_32(A)
#  endif
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
