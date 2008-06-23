//

#ifndef ZTR_UTIL_HH
#define ZTR_UTIL_HH

#define FALSE 0
#define TRUE 1
#define ABANDON
#define ADOPT

#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <endian.h>
#include <srf_config.h>

// put in a global include
#define FALSE 0
#define TRUE 1
#define ABANDON
#define ADOPT

#define ZTR_METADATA_VALTYPE "VALTYPE"
#define ZTR_METADATA_NAME "NAME"

#if WORDS_BIGENDIAN == 1

  #define ZTR_ByteSwapAsRequired_4(A) (A)
  #define ZTR_ByteSwapAsRequired_2(A) (A)

#else

  #define ZTR_ByteSwapAsRequired_2(A) ((((uint16_t)(A) & 0xff00) >> 8) | \
                              (((uint16_t)(A) & 0x00ff) << 8))
  #define ZTR_ByteSwapAsRequired_4(A)  ((((uint32_t)(A) & 0xff000000) >> 24) | \
                              (((uint32_t)(A) & 0x00ff0000) >> 8)  | \
                              (((uint32_t)(A) & 0x0000ff00) << 8)  | \
                              (((uint32_t)(A) & 0x000000ff) << 24))

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
