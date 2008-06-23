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
#include <SRF_XMLBlock.hh>

SRF_XMLBlock::SRF_XMLBlock( void )
: SRF_Block( SRF_BlockTypeXMLBlock )
{
    // no-op
}
    

SRF_XMLBlock::~SRF_XMLBlock( void )
{
    // no-op
}

bool
SRF_XMLBlock::populate( void )
{
    // the data is comprised of the xml string only
    xmlString = data;

   return TRUE;
}

void
SRF_XMLBlock::dump( void ) const
{
    SRF_Block::dump();
}

bool
SRF_XMLBlock::refreshData( void )
{
    data = xmlString;

    return TRUE;
}

int
SRF_XMLBlock::headerSize( void ) const
{
     return 1;
}

int
SRF_XMLBlock::bytesForBlockSize( void ) const
{
    return 4;
}

uint32_t
SRF_XMLBlock::calcDataSize( void ) const
{
    uint32_t dataSize = xmlString.size();

    return dataSize;
}

const std::string&
SRF_XMLBlock::getXmlString( void ) const
{
    return xmlString;
}

bool
SRF_XMLBlock::setup( const std::string& xmlStringIn )
{
    xmlString = xmlStringIn;

    return TRUE;
}
