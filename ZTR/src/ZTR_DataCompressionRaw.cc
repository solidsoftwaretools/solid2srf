///
#include <ios>
#include <string>
#include <sstream>
#include <ZTR_DataCompressionRaw.hh>
#include <ZTR_Chunk.hh>
#include <ZTR_Types.hh>

ZTR_DataCompressionRaw::ZTR_DataCompressionRaw( void )
{
    // no-op
}
    
ZTR_DataCompressionRaw::~ZTR_DataCompressionRaw( void )
{
    // no-op
}

bool
ZTR_DataCompressionRaw::byteOrderAndUncompress(
                                    const std::string& compressedData,
                                    ZTR_Chunk *theChunk ) const
{
    // raw = no compression
    ZTR_Type type = theChunk->getDataType();
    int bytesInType = ZTR_TypeNumberOfByte( type );
    int dataLength = compressedData.length();

    if ( dataLength % bytesInType != 0 )
    {
        ZTR_ReportError( "data length not divisible by byte in type" );
        return FALSE;
    }

    if ( type == ZTR_TypeString )
    {
        theChunk->setUncompressedDataStr( compressedData );
    }
    else if ( type == ZTR_Type32bitInt ||
              type == ZTR_Type16bitInt ||
              type == ZTR_Type8bitUnsignedInt )
    {
        std::vector<int> dataIntVec;

        std::istringstream inputData( compressedData,
                          std::istringstream::in|std::istringstream::binary );

        char* buffer = new char[4];
        inputData.read( buffer, bytesInType );
        while ( !inputData.eof() )
        {
             int value = 0;
             if ( type == ZTR_Type32bitInt )
             {
                  value = ZTR_ConvertByteOrderedBufferTo32bitInt( buffer );
             }
             else if ( type == ZTR_Type16bitInt )
             {
                  value = ZTR_ConvertByteOrderedBufferTo16bitInt( buffer );
             }
             else if ( type == ZTR_Type8bitUnsignedInt )
             {
                  value = (uint8_t) buffer[0];
             }

             dataIntVec.push_back( value );
             inputData.read( buffer, bytesInType );
         }
         DELETE_ARRAY( buffer );

         theChunk->setUncompressedDataInt( dataIntVec );
    }
    else if ( type == ZTR_Type32bitFloat ||
              type == ZTR_Type16bitFloat )
    {
        std::vector<float> dataFloatVec;

        std::istringstream inputData( compressedData,
                          std::istringstream::in|std::istringstream::binary );

        char* buffer = new char[4];
        inputData.read( buffer, bytesInType );
        while ( !inputData.eof() )
        {
             float value = 0;
             if ( type == ZTR_Type32bitFloat )
             {
                  value = ZTR_ConvertByteOrderedBufferTo32bitFloat( buffer );
             }
             else if ( type == ZTR_Type16bitFloat )
             {
                  value = ZTR_ConvertByteOrderedBufferTo16bitFloat( buffer );
             }

             dataFloatVec.push_back( value );
             inputData.read( buffer, bytesInType );
         }
         DELETE_ARRAY( buffer );

         theChunk->setUncompressedDataFloat( dataFloatVec );
    }
    else
    {
        ZTR_ReportError( "unknown type" );
        return FALSE;
    }

    return TRUE;
}

bool
ZTR_DataCompressionRaw::compressAndByteOrder(
                    const ZTR_Chunk* theChunk,
                    std::string* compressedData ) const
{
    // raw = no compression
    ZTR_Type type = theChunk->getDataType();

    if ( type == ZTR_TypeString )
    {
        *compressedData = theChunk->getUncompressedDataStr();
    }
    else if ( type == ZTR_Type32bitInt )
    {
        const std::vector<int> dataIntVec = theChunk->getUncompressedDataInt();

        std::vector<int>::const_iterator it = dataIntVec.begin();

        while( it < dataIntVec.end() )
        {
             int value = *it;
             value = ZTR_ByteSwapAsRequired_4( value );
             compressedData->append(
                            reinterpret_cast<const char*>(&value), 4 );
             it++;
        }
    }
    else if ( type == ZTR_Type16bitInt )
    {
        const std::vector<int> dataIntVec = theChunk->getUncompressedDataInt();

        std::vector<int>::const_iterator it = dataIntVec.begin();

        while( it < dataIntVec.end() )
        {
             short value = *it;
             value = ZTR_ByteSwapAsRequired_2( value );
             compressedData->append(
                            reinterpret_cast<const char*>(&value), 2 );
             it++;
        }
    }
    else if ( type == ZTR_Type8bitUnsignedInt )
    {
        const std::vector<int> dataIntVec = theChunk->getUncompressedDataInt();

        std::vector<int>::const_iterator it = dataIntVec.begin();

        while( it < dataIntVec.end() )
        {
             short value = *it;
             unsigned char valueStr = (unsigned char) value;
             (*compressedData) += valueStr;
             it++;
        }
    }
    else if ( type == ZTR_Type32bitFloat )
    {
        const std::vector<float> dataFloatVec =
                                          theChunk->getUncompressedDataFloat();

        std::vector<float>::const_iterator it = dataFloatVec.begin();

        while( it < dataFloatVec.end() )
        {
             typedef union
             {
                 float f;
                 uint32_t i;
             } ZTR_floatToIntU;
             ZTR_floatToIntU conv;
             conv.f = *it;
             uint32_t value = ZTR_ByteSwapAsRequired_4( conv.i );
			 // FIXME - Expensive string appending.
             compressedData->append(
                            reinterpret_cast<const char*>(&value), 4 );
             it++;
        }
    }
    else
    {
        ZTR_ReportError( "unknown type" );
        return FALSE;
    }

    return TRUE;
}

ZTR_DataCompressionType
ZTR_DataCompressionRaw::getDataCompressionType( void ) const
{
    return ZTR_DataCompressionTypeRaw;
}

