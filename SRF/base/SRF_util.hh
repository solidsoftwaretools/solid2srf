//Copyright 2007 Asim Siddiqui
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//

#ifndef SRF_UTIL_HH
#define SRF_UTIL_HH
//
// file: SRF_util.h
// Purpose: Header file for container writer class
// Author: Asim Siddiqui
// Update History
// 18th March 2007  Asim Siddiqui        created
//

#ifdef HAVE_CONFIG_H
#  include "srf_config.h"
#endif

#include <iosfwd>
#include <string>
#include <vector>

#ifdef HAVE_STDINT_H
#   include <stdint.h>
#endif

#ifdef HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#else /* !HAVE_SYS_ENDIAN_H */
#ifdef HAVE_MACHINE_ENDIAN_H
#include <machine/endian.h>
#else /* !HAVE_MACHINE_ENDIAN_H */
#ifdef HAVE_ENDIAN_H
#include <endian.h>
#else /* !HAVE_ENDIAN_H */
#error "No supported endian.h"
#endif /* !HAVE_ENDIAN_H */
#endif /* !HAVE_MACHINE_ENDIAN_H */
#endif /* !HAVE_SYS_ENDIAN_H */

#ifndef FALSE
#  define FALSE false
#endif
#ifndef TRUE
#  define TRUE true
#endif

#define ABANDON
#define ADOPT

ADOPT char* SRF_cStrToPascalStr( const char* str );
void SRF_readString( std::istringstream* input, std::string& readString );
void SRF_ReportError( const std::string& errMesg );

#if defined( WORDS_BIGENDIAN ) && WORDS_BIGENDIAN == 1

  #define hostToFileShort(A) (A)
  #define hostToFileLong(A) (A)
  #define fileToHostShort(A) (A)
  #define fileToHostLong(A) (A)

#else

  #define hostToFileShort(A) ((((uint16_t)(A) & 0xff00) >> 8) | \
                              (((uint16_t)(A) & 0x00ff) << 8))
  #define hostToFileLong(A)  ((((uint32_t)(A) & 0xff000000) >> 24) | \
                              (((uint32_t)(A) & 0x00ff0000) >> 8)  | \
                              (((uint32_t)(A) & 0x0000ff00) << 8)  | \
                              (((uint32_t)(A) & 0x000000ff) << 24))

  #define fileToHostShort  hostToFileShort
  #define fileToHostLong   hostToFileLong

#endif
#endif
