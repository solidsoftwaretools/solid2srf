//
#include <iostream>
#include <string>
#include <cstdlib>

#include "SRF_util.hh"
#include "SRF_SOLiDstructs.hh"
#include "SRF_SOLiDcfastaFile.hh"

SRF_SOLiDcfastaFile::SRF_SOLiDcfastaFile( void )
{
    // no-op
}
    
SRF_SOLiDcfastaFile::~SRF_SOLiDcfastaFile( void )
{
    // no-op
}

bool
SRF_SOLiDcfastaFile::readNextBlock( SRF_SOLiDdataSet* dataSet )
{
    if ( (*file).peek() != '>' )
    {
        std::cerr << "ERROR: unexpected file format in cfasta file\n";
	std::string line;
	std::getline( (*file), line);
	std::cerr << "BADDATA: " << line << std::endl;
	std::getline( (*file), line);
	std::cerr << "BADDATA: " << line << std::endl;
        return FALSE;
    }

    char tmp;
    *file >> tmp >> dataSet->partialReadId;
    (*file).get(); // new line char

    // get panel Id
    int firstUnderscore = dataSet->partialReadId.find( '_' );
    if ( firstUnderscore == -1 )
    {
        std::cerr << "ERROR: unexpected file format at Read Id:" <<
                dataSet->partialReadId << std::endl;
        exit(-1);
    }

    dataSet->panelId = dataSet->partialReadId.substr( 0, firstUnderscore + 1 );
    dataSet->readIdSuffix =
                       dataSet->partialReadId.substr( firstUnderscore + 1 );

    // next is the line of calls
    std::getline( *file, dataSet->calls );

    return TRUE;
}
