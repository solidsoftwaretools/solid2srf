//
#include <ios>
#include <string>
#include <ZTR_DataCompression.hh>
#include <ZTR_DataCompressionRaw.hh>

#define ZTR_COMPRESSION_TYPE_RAW 0

ZTR_DataCompression::ZTR_DataCompression( void )
{
    // no-op
}
    
ZTR_DataCompression::~ZTR_DataCompression( void )
{
    // no-op
}

ZTR_DataCompressionType
ZTR_DataCompression::getDataCompressionType( void ) const
{
    return ZTR_DataCompressionTypeUnspecified;
}

int
ZTR_DataCompressionTypeToFormat( ZTR_DataCompressionType type )
{
    if ( type == ZTR_DataCompressionTypeRaw )
    {
        return ZTR_COMPRESSION_TYPE_RAW;
    }

    ZTR_ReportError( "unknown compression type" );
    return -1;
}


ADOPT ZTR_DataCompression*
ZTR_GetNewDataCompression( int format )
{
    ZTR_DataCompression* newDataCompression = NULL;

    if ( format == ZTR_COMPRESSION_TYPE_RAW )
    {
        newDataCompression = new ZTR_DataCompressionRaw;
    }
    else
    {
        ZTR_ReportError( "unknown compression type" );
        return NULL;
    }

    return newDataCompression;
}

/*
bool
ZTR_DataCompressionRaw::byteOrderAndUncompress(
                                    const std::string& compressedData,
                                    std::string* unpackedData ) const
{
    return FALSE;
}

bool
ZTR_DataCompressionRaw::compressAndByteOrder( const std::string& unpackedData,
                                  std::string* compressedData ) const
{
    return FALSE;
}
*/
