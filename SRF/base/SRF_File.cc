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
#include <iostream>
#include <fstream>
#include <SRF_File.hh>
#include <SRF_util.hh>

SRF_File::SRF_File( const char* filenameIn, SRF_FileOpenType openParamIn )
{
     filename = filenameIn;
     openParam = openParamIn;
}

bool
SRF_File::open ( void )
{
    if ( openParam == SRF_FileOpenTypeRead )
    {
        file.open(filename.data(), std::fstream::in|std::fstream::binary );
    }
    else if ( openParam == SRF_FileOpenTypeWrite )
    {
        file.open(filename.data(), std::fstream::out|std::fstream::binary );
    }
    else if ( openParam == SRF_FileOpenTypeReadWrite )
    {
        file.open(filename.data(), std::fstream::in|std::fstream::out|std::fstream::binary );
    }
    else
    {
        std::string errMesg = "unknown open type";
        errMesg.append( filename );
        SRF_ReportError( errMesg );

        return FALSE;
    }

    if ( !file.is_open() )
    {
        std::string errMesg = "can't open file ";
        errMesg.append( filename );
        SRF_ReportError( errMesg );

        return FALSE;
    }

    return TRUE;
}


SRF_File::~SRF_File( void )
{
    file.close();
}

std::fstream&
SRF_File::getFile( void )
{
    return file;
}

