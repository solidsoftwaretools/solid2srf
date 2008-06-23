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
