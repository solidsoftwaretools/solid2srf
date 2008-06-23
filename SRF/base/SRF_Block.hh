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

#ifndef SRF_BLOCK_HH
#define SRF_BLOCK_HH
//
// file: SRF_Block.hh
// Purpose: Abstract base class for SRF blocks
//
#include <fstream>
#include <string>
#include <SRF_util.hh>

typedef enum {
    SRF_BlockTypeNone,
    SRF_BlockTypeContainerHeader,
    SRF_BlockTypeXMLBlock,
    SRF_BlockTypeDataBlock,
    SRF_BlockTypeDataBlockHeader,
    SRF_BlockTypeIndexBlock,
    SRF_BlockTypeNullIndex
} SRF_BlockType;

class SRF_Block
{
    public:
        SRF_Block( SRF_BlockType blockTypeIn );

        virtual ~SRF_Block( void );

        bool write( std::fstream& file );

        virtual void dump( void ) const;

        SRF_BlockType getBlockType( void ) const;



        virtual int headerSize() const = 0;
        virtual int bytesForBlockSize() const = 0;

    protected:
        virtual bool populate( void ) = 0;
        virtual uint32_t calcDataSize( void ) const = 0;
        virtual bool refreshData( void ) = 0;

        uint32_t getBlockSize( void ) const;

        std::string        data;

    private:
        SRF_Block( void );

        bool writeCommonData( void );
        bool refreshCommonData( void );

        bool blockTypeInFile( std::string& blockTypeAsString ) const;

        SRF_BlockType blockType;
        uint32_t blockSize;

        friend SRF_BlockType SRF_GetNewBlock( std::fstream& file,
                                              ADOPT SRF_Block** newBlock );
};

SRF_BlockType
SRF_GetNewBlock( std::fstream& file, ADOPT SRF_Block** newBlock );

#endif
