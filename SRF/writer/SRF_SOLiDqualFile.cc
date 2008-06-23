//
#include <map>
#include <string>
#include <ZTR_util.hh>
#include <SRF_util.hh>
#include <SRF_SOLiDstructs.hh>
#include <SRF_SOLiDqualFile.hh>

SRF_SOLiDqualFile::SRF_SOLiDqualFile( void )
{
    // no op
}
    
SRF_SOLiDqualFile::~SRF_SOLiDqualFile( void )
{
    // no-op
}

bool
SRF_SOLiDqualFile::readNextBlock( const std::string& cfastaPartialReadId,
                                  SRF_SOLiDdataSet* dataSet )
{
    if ( !isFileOpen() )
    {
        return TRUE;
    }

    if ( !readNextBlockCommon( cfastaPartialReadId ) )
    {
       return FALSE;
    }

    // next is the line of quals
    dataSet->confValues.clear();
    std::string confValuesString;
    std::getline( file, confValuesString );
    ZTR_ConvertStringToIntVector( confValuesString, &(dataSet->confValues) );

    return TRUE;
}
