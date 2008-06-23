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
#ifndef SRF_XMLBLOCK_HH
#define SRF_XMLBLOCK_HH
//
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <SRF_Block.hh>
#include <SRF_util.hh>

class SRF_XMLBlock : public SRF_Block
{
    public:
        SRF_XMLBlock( void );
        ~SRF_XMLBlock( void );

        bool setup( const std::string& xmlStringIn );
        virtual void dump( void ) const;

        int headerSize() const;
        int bytesForBlockSize() const;

        const std::string& getXmlString( void ) const;

    protected:
        bool populate( void );
        uint32_t calcDataSize( void ) const;
        bool refreshData( void );

    private:
       std::string xmlString;

};

#endif
