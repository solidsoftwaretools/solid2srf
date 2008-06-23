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
#include <SRF_Read.hh>
#include <SRF_ContainerHeader.hh>
#include <SRF_DataBlockHeader.hh>
#include <SRF_DataBlock.hh>

SRF_Read::SRF_Read( void )
{
  // null
}

SRF_Read::SRF_Read( SRF_ContainerHeader* containerHeaderPtr,
                    SRF_DataBlockHeader* dataBlockHeaderPtr,
                    ABANDON SRF_DataBlock* dataBlockPtr )
{
    containerHeader = containerHeaderPtr;
    dataBlockHeader = dataBlockHeaderPtr;
    dataBlock = dataBlockPtr;

    readId = dataBlockHeader->getPrefix() + dataBlock->getShortId();

    data = dataBlockHeaderPtr->getHeader() + dataBlock->getBlob();
}

SRF_Read::~SRF_Read( void )
{
    delete dataBlock;
}    

const SRF_ContainerHeader*
SRF_Read::containerHeaderPtr( void ) const
{
    return containerHeader;
}

const SRF_DataBlockHeader*
SRF_Read::dataBlockHeaderPtr( void ) const
{
    return dataBlockHeader;
}

const SRF_DataBlock*
SRF_Read::dataBlockPtr( void ) const
{
    return dataBlock;
}

const std::string&
SRF_Read::getReadId( void ) const
{
    return readId;
}

const std::string&
SRF_Read::getData( void ) const
{
    return data;
}
