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
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <SRF_util.hh>
#include <SRF_DataBlock.hh>

SRF_DataBlock::SRF_DataBlock( void )
: SRF_Block( SRF_BlockTypeDataBlock )
{
    readFlags = '\0';
}
    

SRF_DataBlock::~SRF_DataBlock( void )
{
}

bool
SRF_DataBlock::populate( void )
{
    // need to read from data object
    std::istringstream inputData( data, std::istringstream::in|std::istringstream::binary );
  
    inputData.read ( &readFlags, 1);
    SRF_readString( &inputData, shortId );

    uint32_t sizeOfBlobCalc = sizeOfBlob();
    if ( sizeOfBlobCalc < 0 )
    {
        SRF_ReportError( "invalid block size for read" );
        return FALSE;
    }

    blob.clear();
    if ( sizeOfBlobCalc > 0 )
    {
        char cstr[sizeOfBlobCalc];
        inputData.read( cstr,  sizeOfBlobCalc);
        blob.assign(cstr,  sizeOfBlobCalc);
    }

   return TRUE;
}

void
SRF_DataBlock::dump( void ) const
{
    SRF_Block::dump();

    std::cout << "ShortId:" << shortId << std::endl;
    std::cout << "Size of blob:" << sizeOfBlob() <<std::endl;

    //readFlags??
}

bool
SRF_DataBlock::refreshData( void )
{
    // write to data object as a stream
    std::ostringstream outputData( std::ostringstream::out|std::ostringstream::binary );

    char* shortIdStr = ADOPT SRF_cStrToPascalStr( shortId.c_str() );

    outputData << readFlags;
    outputData << shortIdStr;
    outputData << blob;

    data = outputData.str();

    delete [] shortIdStr;

    return TRUE;
}

int
SRF_DataBlock::headerSize( void ) const
{
     return 1;
}

int
SRF_DataBlock::bytesForBlockSize( void ) const
{
    return 4;
}

uint32_t
SRF_DataBlock::calcDataSize( void ) const
{
    int dataSize = shortId.length() + 1;
    dataSize += 1; // readFlags
    dataSize += blob.length();

    return dataSize;
}

uint32_t
SRF_DataBlock::sizeOfBlob( void ) const
{
    uint32_t sizeHeaderBlob = getBlockSize() - headerSize()
                                 - bytesForBlockSize()
                                 - shortId.length() - 1
                                 -1; // readFlags

    return sizeHeaderBlob;
}

const std::string&
SRF_DataBlock::getShortId( void ) const
{
    return shortId;
}

const std::string&
SRF_DataBlock::getBlob( void ) const
{
    return blob;
}

bool
SRF_DataBlock::setup( const std::string& shortIdIn,
                      const std::string& blobIn, 
                      char readFlagsIn )
{
    shortId = shortIdIn;
    blob = blobIn;
    readFlags = readFlagsIn;

    return TRUE;
}
