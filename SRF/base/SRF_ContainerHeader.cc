//Copyright 2008 Asim Siddiqui
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
//
//
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <SRF_util.hh>
#include <SRF_ContainerHeader.hh>

#define CONTAINERTYPEZTR 'Z'
#define FORMATVERSION "1.3"

SRF_ContainerHeader::SRF_ContainerHeader( void )
: SRF_Block( SRF_BlockTypeContainerHeader )
{
        formatVersion = FORMATVERSION;
        containerType = SRF_ContainerTypeNone;
}

SRF_ContainerHeader::~SRF_ContainerHeader( void )
{
   // null
}    

bool
SRF_ContainerHeader::populate( void )
{
    std::istringstream inputData( data,
                         std::istringstream::in|std::istringstream::binary );
    SRF_readString( &inputData, formatVersion );
    if ( formatVersion.compare( FORMATVERSION ) != 0 )
    {
        SRF_ReportError( "incorrect format version" );
        return FALSE;
    }
  
    char tmp;
    inputData.read( &tmp, 1);
    if ( tmp == CONTAINERTYPEZTR )
    {
        containerType = SRF_ContainerTypeZTR;
    }
    else
    {
        SRF_ReportError( "unsupported container type" );
        return FALSE;
    }

    SRF_readString( &inputData, baseCaller );
    SRF_readString( &inputData, baseCallerVersion );

    return TRUE;
}

bool
SRF_ContainerHeader::setup( SRF_ContainerType containerTypeIn,
                      const std::string& baseCallerIn,
                      const std::string& baseCallerVersionIn )
{
    containerType = containerTypeIn;
    baseCaller = baseCallerIn;
    baseCallerVersion = baseCallerVersionIn;

    return TRUE;
}

char
SRF_ContainerHeader::getContainerType( void ) const
{
    return containerType;
}

const std::string&
SRF_ContainerHeader::getBaseCaller( void ) const
{
    return baseCaller;
}

const std::string&
SRF_ContainerHeader::getBaseCallerVersion( void ) const
{
    return baseCallerVersion;
}

const std::string&
SRF_ContainerHeader::getFormatVersion( void ) const
{
    return formatVersion;
}

bool
SRF_ContainerHeader::refreshData( void )
{
    std::ostringstream outputData(
                           std::ostringstream::out|std::ostringstream::binary );

    char* formatVersionStr = ADOPT SRF_cStrToPascalStr( formatVersion.c_str() );
    char containerTypeChar;
    if ( containerType == SRF_ContainerTypeZTR )
    {
        containerTypeChar = CONTAINERTYPEZTR;
    }

    char* baseCallerStr = ADOPT SRF_cStrToPascalStr( baseCaller.c_str() );
    char* baseCallerVersionStr = ADOPT SRF_cStrToPascalStr( baseCallerVersion.c_str() );

    outputData << formatVersionStr;
    outputData << containerTypeChar;
    outputData << baseCallerStr;
    outputData << baseCallerVersionStr;

    data = outputData.str();

    delete [] formatVersionStr;
    delete [] baseCallerStr;
    delete [] baseCallerVersionStr;

    return TRUE;
}

void
SRF_ContainerHeader::dump( void ) const
{
    // call base method
    SRF_Block::dump();

    std::cout << "Format Version:" << formatVersion  <<std::endl;
    std::cout << "Container Type:" << containerType <<std::endl;
    std::cout << "Base Caller:" << baseCaller  <<std::endl;
    std::cout << "Base Caller Version:" << baseCallerVersion  <<std::endl;
}

int
SRF_ContainerHeader::headerSize( void ) const
{
     return 4;
}

int
SRF_ContainerHeader::bytesForBlockSize( void ) const
{
    return 4;
}

uint32_t
SRF_ContainerHeader::calcDataSize( void ) const
{
    int dataSize = formatVersion.length() + 1;
    dataSize += 1; // containerType
    dataSize += baseCaller.length() + 1;
    dataSize += baseCallerVersion.length() + 1;

    return dataSize;
}
