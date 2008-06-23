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

#ifndef SRF_READ_HH
#define SRF_READ_HH
//
#include <string>
#include <SRF_util.hh>

class SRF_ContainerHeader;
class SRF_DataBlockHeader;
class SRF_DataBlock;

class SRF_Read
{
    public:
        SRF_Read( void );
        SRF_Read( SRF_ContainerHeader* containerHeaderPtr,
                    SRF_DataBlockHeader* dataBlockHeaderPtr,
                    ABANDON SRF_DataBlock* dataBlockPtr );

        virtual ~SRF_Read( void );

        const std::string& getReadId( void ) const;
        const std::string& getData( void ) const;

        const SRF_ContainerHeader* containerHeaderPtr( void ) const;
        const SRF_DataBlockHeader* dataBlockHeaderPtr( void ) const;
        const SRF_DataBlock* dataBlockPtr( void ) const;

    private:
        const SRF_ContainerHeader* containerHeader;
        const SRF_DataBlockHeader* dataBlockHeader;
        const SRF_DataBlock* dataBlock;

        std::string readId;
        std::string data;
};

#endif
