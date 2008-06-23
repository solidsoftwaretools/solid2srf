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
#ifndef SRF_DATABLOCK_HH
#define SRF_DATABLOCK_HH
//
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <SRF_Block.hh>
#include <SRF_util.hh>

class SRF_DataBlock : public SRF_Block
{
    public:
        SRF_DataBlock( void );
        ~SRF_DataBlock( void );

        bool setup( const std::string& shortIdIn, 
                    const std::string& blobIn,
                    char readFlagsIn = '\0' );

        virtual void dump( void ) const;

        int headerSize() const;
        int bytesForBlockSize() const;

        const std::string& getShortId( void ) const;
        const std::string& getBlob( void ) const;

    protected:
        bool populate( void );
        uint32_t calcDataSize( void ) const;
        bool refreshData( void );

    private:
        uint32_t sizeOfBlob( void ) const;

        std::string blob;
        std::string shortId;
        char readFlags;
};

#endif
