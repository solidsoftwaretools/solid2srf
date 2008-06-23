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
// file: SsrfUtil.h
// Purpose: Header file for container writer class
// Author: Asim Siddiqui
// Update History
// 18th March 2007  Asim Siddiqui        created
// Some code contributed by James Bonfield and under a different OS license
//
#include <ios>
#include <string>
#include <iostream>
#include <sstream>

#include <SRF_util.hh>

using namespace std;

// creates a pascal style string. The first char is the length of the string
ADOPT char*
SRF_cStrToPascalStr( const char* str )
{
    int len;
    unsigned char lengthStr;
    char* newStr;
    char* strPtr;

    len = strlen( str );
    if ( len > 255 )
    {
        printf("ERROR: String too long\n");
        exit(1);
    }

    lengthStr = (unsigned char) len;

    newStr = new char[ len + 2 ];

    strPtr = newStr;
    *strPtr = lengthStr;
    strPtr++;
    strcpy( strPtr, str );

    return newStr;
}

void
SRF_readString( std::istringstream* input, std::string& readString )
{
    int len;
    char lenStr;    

    input->read( &lenStr, 1);

    len = (int) lenStr;

//    std::cout << len <<std::endl;

    char* str = new char[len];

    input->read( str, len );

    readString.assign( str, len );
    delete [] str;
}

void
SRF_ReportError( const std::string& errorMesg )
{
    std::cout << "ERROR: SRF package:" << errorMesg << std::endl;
}
