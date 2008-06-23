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
#ifndef SRF_CONTAINERHEADER_HH
#define SRF_CONTAINERHEADER_HH
//
#include <ios>
#include <string>
#include <SRF_Block.hh>
#include <SRF_util.hh>

typedef enum {
    SRF_ContainerTypeNone,
    SRF_ContainerTypeZTR
} SRF_ContainerType;

class SRF_ContainerHeader : public SRF_Block
{
    public:
        SRF_ContainerHeader( void );
        ~SRF_ContainerHeader( void );

        bool setup( SRF_ContainerType containerTypeIn,
                    const std::string& baseCallerIn,
                    const std::string& baseCallerVersionIn );


        virtual void dump( void ) const;


        int headerSize() const;
        int bytesForBlockSize() const;

        char getContainerType( void ) const;
        const std::string& getBaseCaller( void ) const;
        const std::string& getBaseCallerVersion( void ) const;
        const std::string& getFormatVersion( void ) const;

    protected:
        virtual bool populate( void );
        virtual uint32_t calcDataSize( void ) const;
        virtual bool refreshData( void );

    private:
        std::string baseCaller;
        std::string baseCallerVersion;
        std::string formatVersion;
        SRF_ContainerType containerType;
};

#endif
