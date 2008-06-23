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

#ifndef SRF_FILE_HH
#define SRF_FILE_HH
//
// file: SRF_File.hh
// Purpose: Header file for container class
// Author: Asim Siddiqui
// Update History
// 15th April 2007  Asim Siddiqui        created
//
#include <ios>
#include <fstream>
#include <vector>
#include <string>

typedef enum
{
    SRF_FileOpenTypeRead,
    SRF_FileOpenTypeWrite,
    SRF_FileOpenTypeReadWrite,
    SRF_FileOpenTypeAppend
} SRF_FileOpenType;

class SRF_File
{
    public:
        SRF_File( const char* filenameIn, SRF_FileOpenType openParamIn );
        ~SRF_File( void );

        std::fstream& getFile();

       bool open( void );
       bool close( void );

       bool start( void );
       bool end( void );

       bool concatenate( void );
       bool indexed( void );
       bool multiFile( void ) const;

    private:
       bool prepForAppend( void );

       std::fstream file;
       std::string filename;
       SRF_FileOpenType openParam;
};

#endif
