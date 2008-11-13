//
#include <iostream>
#include <string>
#include <sstream>
#include "ZTR_util.hh"


const char* const ZTR_MAGIC_NUMBER = "\256ZTR\r\n\032\n";

void
ZTR_ReportError( const std::string& errMesg )
{
    std::cout << "ERROR: ZTR package: " << errMesg << std::endl;
}

// TO DO temporary location - will be moved after introduction of mixing
// matrices
const std::string
ZTR_Header( void )
{
    std::string header = ZTR_MAGIC_NUMBER;
    uint8_t major = 1;
    header.append( reinterpret_cast<const char *>(&major), 1 );
    uint8_t minor = 4;
    header.append( reinterpret_cast<const char *>(&minor), 1 );

    return header;
}

void
ZTR_ConvertStringToIntVector( const std::string& valueString,
                              std::vector<int>* valueVec )
{
    std::istringstream inputData( valueString );

    while (inputData.peek() != EOF )
    {
        int value;
        inputData >> value;
        if ( inputData.good() )
        {
            valueVec->push_back( value );
        }
    }
}

void
ZTR_ConvertStringToFloatVector( const std::string& valueString,
                                std::vector<float>* valueVec )
{
    std::istringstream inputData( valueString );

    while (inputData.peek() != EOF )
    {
        float value;
        inputData >> value;
        if ( inputData.good() )
        {
            valueVec->push_back( value );
        }
    }
}
