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

#include <iostream>
#include <string>

#include "SRF_util.hh"
#include "SRF_Block.hh"
#include "SRF_ContainerHeader.hh"
#include "SRF_DataBlockHeader.hh"
#include "SRF_DataBlock.hh"
#include "SRF_XMLBlock.hh"
#include "SRF_IndexBlock.hh"

#define CONTAINERHEADERCODE "SSRF"
#define CONTAINERHEADERCODEFIRSTLETTER 'S'
#define INDEXCODE "I   "
#define INDEXCODEFIRSTLETTER 'I'
#define XMLBLOCKCODE 'X'
#define DATABLOCKCODE 'R'
#define DATABLOCKHEADERCODE 'H'
#define NULL_INDEX_FIRST_LETTER '\000'
#define NULL_INDEX_CODE "\000\000\000\000\000\000\000\000"

SRF_Block::SRF_Block( void )
{
        blockType = SRF_BlockTypeNone;
        blockSize = 0;
}

SRF_Block::SRF_Block( SRF_BlockType blockTypeIn )
{
        blockType = blockTypeIn;
        blockSize = 0;
}

SRF_Block::~SRF_Block( void )
{
    // no op
}    

SRF_BlockType
SRF_GetNewBlock( std::fstream& file, ADOPT SRF_Block** newBlock )
{
    *newBlock = NULL;
    char blockTypeIn;
    file.read(&blockTypeIn, 1);

    if (blockTypeIn == CONTAINERHEADERCODEFIRSTLETTER )
    {
        char tmp[5];
        tmp[0] = blockTypeIn;
        tmp[4] = '\0';
        file.read(&(tmp[1]), 3);
        if ( strcmp( tmp, CONTAINERHEADERCODE ) != 0 )
        {
            SRF_ReportError( "unknown block type" );
            return SRF_BlockTypeNone;
        }

        *newBlock = new SRF_ContainerHeader;
    }
//    else if ( blockTypeIn == INDEXCODEFIRSTLETTER )
//    {
//        char tmp[4];
//        file.read(tmp, 4);
//        if ( strcmp( tmp, INDEXCODE ) != 0 )
//        {
//            SRF_ReportError( "unknown block type" );
//            return FALSE;
//        }
//
//        *newBlock = new SRF_IndexBlock;
//    }
    else if ( blockTypeIn == XMLBLOCKCODE )
    {
        *newBlock = new SRF_XMLBlock;
    }
    else if ( blockTypeIn == DATABLOCKCODE )
    {
        *newBlock = new SRF_DataBlock;
    }
    else if ( blockTypeIn == DATABLOCKHEADERCODE )
    {
        *newBlock = new SRF_DataBlockHeader;
    }
    else if ( blockTypeIn == NULL_INDEX_FIRST_LETTER )
    {
        char tmp[8];
        tmp[0] = blockTypeIn;
        file.read(&(tmp[1]), 7);
        char buffer[8];
        memset( buffer, 0, 8 );
        std::string nullCode;
        nullCode.assign( buffer, 8 );
        std::string codeRead;
        codeRead.assign( tmp, 8 );
        if ( nullCode.compare( codeRead ) == 0 )
        {
            return SRF_BlockTypeNullIndex;
        }

        SRF_ReportError( "unknown block type" );
        return SRF_BlockTypeNone;
    }
    else
    {
        return SRF_BlockTypeNone;
    }

    uint32_t dataSize = 0;
    if ( (*newBlock)->blockType == SRF_BlockTypeIndexBlock )
    {
        // need to add 64 bit support
        // index will need to be handled differently as data cannot be read into memory
    }
    else
    {
        file.read( reinterpret_cast<char *>(&((*newBlock)->blockSize)), 4 );
        (*newBlock)->blockSize = fileToHostLong((*newBlock)->blockSize);
    }

    dataSize = (*newBlock)->blockSize - (*newBlock)->headerSize() - (*newBlock)->bytesForBlockSize();

    char* dataBuffer = new char[dataSize];
    file.read( dataBuffer, dataSize );

    (*newBlock)->data.assign( dataBuffer, dataSize );
    delete [] dataBuffer;

    if ( !(*newBlock)->populate() )
    {
        SRF_ReportError( "unknown block type" );
        delete (*newBlock);
        return SRF_BlockTypeNone;
    }

    return (*newBlock)->getBlockType();
}

SRF_BlockType
SRF_Block::getBlockType( void ) const
{
    return blockType;
}

bool
SRF_Block::refreshCommonData()
{
    data.clear();

    uint32_t dataSize = calcDataSize();
    blockSize = dataSize + headerSize() + bytesForBlockSize();

    return TRUE;
}

bool
SRF_Block::write( std::fstream& file )
{
    if ( !refreshCommonData() )
    {
        SRF_ReportError( "could not refresh common data" );
        return FALSE;
    }
    if ( !refreshData() )
    {
        SRF_ReportError( "could not refresh data before writing block" );
        return FALSE;
    }
    //write this class and then subclass
    std::string blockTypeAsString;
    if ( !blockTypeInFile( blockTypeAsString ) )
    {
        SRF_ReportError( "unknown block type" );
        return FALSE;
    }

    file << blockTypeAsString;

    if ( blockType != SRF_BlockTypeIndexBlock )
    {
        int32_t be_blockSize = hostToFileLong( blockSize );
        file.write(reinterpret_cast<const char*>(&be_blockSize),4);

        file << data;

    }
    else
    {
        SRF_ReportError( "block type not supported" );
        return FALSE;
    }

    return TRUE;
}

void
SRF_Block::dump( void ) const
{
    std::cout << "Block Type:" << blockType <<std::endl;
    std::cout << "Block Size:" << blockSize  <<std::endl;

}

bool
SRF_Block::blockTypeInFile( std::string& blockTypeAsString ) const
{
    
    if (blockType == SRF_BlockTypeContainerHeader )
    {
        blockTypeAsString = CONTAINERHEADERCODE;
    }
//    else if ( blockType == SRF_BlockTypeIndexBlock )
//    {
//        blockTypeAsString = INDEXCODE;
//    }
    else if ( blockType == SRF_BlockTypeXMLBlock )
    {
        blockTypeAsString = XMLBLOCKCODE;
    }
    else if ( blockType == SRF_BlockTypeDataBlock )
    {
        blockTypeAsString = DATABLOCKCODE;
    }
    else if ( blockType == SRF_BlockTypeDataBlockHeader )
    {
        blockTypeAsString = DATABLOCKHEADERCODE;
    }
    else
    {
        SRF_ReportError( "block type not supported" );
        return FALSE;
    }

    return TRUE;
}

uint32_t
SRF_Block::getBlockSize( void ) const
{
    return blockSize;
}
