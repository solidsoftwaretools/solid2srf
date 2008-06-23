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
//
#include <ios>
#include <stddef.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <SRF_DataBlockHeader.hh>


SRF_DataBlockHeader::SRF_DataBlockHeader( void )
: SRF_Block( SRF_BlockTypeDataBlockHeader )
{
     // empty method
}
    

SRF_DataBlockHeader::~SRF_DataBlockHeader( void )
{
     // empty method
}

bool
SRF_DataBlockHeader::populate( void )
{
    std::istringstream inputData( data, std::istringstream::in|std::istringstream::binary );

    char subBlockType; // this field is not used
    inputData.read( &subBlockType, 1 ); 
    SRF_readString( &inputData, prefix );

    uint32_t sizeHeaderBlobCalc = sizeHeaderBlob();

    if ( sizeHeaderBlobCalc < 0 )
    {
        SRF_ReportError( "invalid block size for read set" );
        return FALSE;
    }


    header.clear();
    if ( sizeHeaderBlobCalc > 0 )
    {
	char cstr[sizeHeaderBlobCalc];
        inputData.read( cstr,  sizeHeaderBlobCalc);
	header.assign(cstr,  sizeHeaderBlobCalc);
    }

   return TRUE;
}

bool
SRF_DataBlockHeader::setup( const std::string& prefixIn,
                    const std::string& headerIn )
{
    prefix = prefixIn;
    header = headerIn;

    return TRUE;
}

void
SRF_DataBlockHeader::dump( void ) const
{
    SRF_Block::dump();

    std::cout << "Unique Id Prefix:" << prefix  <<std::endl;
    std::cout << "Size of header blob:" << sizeHeaderBlob() <<std::endl;
}

bool
SRF_DataBlockHeader::refreshData( void )
{
    std::ostringstream outputData( std::ostringstream::out|std::ostringstream::binary );

    outputData << 'E';
    char* str = ADOPT SRF_cStrToPascalStr( prefix.c_str() );

    outputData << str;
    outputData.write(header.data(), header.length());

    data = outputData.str();
    delete [] str;

    return TRUE;
}

int
SRF_DataBlockHeader::headerSize( void ) const
{
     return 1;
}

int
SRF_DataBlockHeader::bytesForBlockSize( void ) const
{
    return 4;
}

uint32_t
SRF_DataBlockHeader::calcDataSize( void ) const
{
    int dataSize = prefix.length() + 1;
    dataSize += header.length();
    dataSize += 1; // subblock type

    return dataSize;
}

uint32_t
SRF_DataBlockHeader::sizeHeaderBlob( void ) const
{
    uint32_t sizeHeaderBlob = getBlockSize() - headerSize()
                                 - bytesForBlockSize()
                                 - 1 // subblock type
                                 - prefix.length() - 1; 

    return sizeHeaderBlob;
}

const std::string&
SRF_DataBlockHeader::getPrefix( void ) const
{
    return prefix;
}

const std::string&
SRF_DataBlockHeader::getHeader( void ) const
{
    return header;
}
