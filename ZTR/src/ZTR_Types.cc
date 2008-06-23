//
#include <ZTR_util.hh>
#include <ZTR_Types.hh>

int
ZTR_TypeNumberOfByte( ZTR_Type type )
{
    int bytesInType = 0;
    if ( type == ZTR_Type32bitInt || type == ZTR_Type32bitFloat )
    {
         bytesInType = 4;
    }
    else if ( type == ZTR_Type16bitInt || type == ZTR_Type16bitFloat )
    {
         bytesInType = 2;
    }
    else if ( type == ZTR_TypeString )
    {
         bytesInType = 1;
    }
    else if ( type == ZTR_Type8bitUnsignedInt )
    {
         bytesInType = 1;
    }

    return bytesInType;
}

int
ZTR_ConvertByteOrderedBufferTo32bitInt( char* buffer )
{
    uint32_t value = *(reinterpret_cast<uint32_t *>(buffer));
    value = ZTR_ByteSwapAsRequired_4( value );

    return value;
}

int
ZTR_ConvertByteOrderedBufferTo16bitInt( char* buffer )
{
    uint16_t value = *(reinterpret_cast<uint16_t *>(buffer));
    value = ZTR_ByteSwapAsRequired_2( value );

    return value;
}


float
ZTR_ConvertByteOrderedBufferTo32bitFloat( char* buffer )
{
    union 
    {
        float f;
        unsigned char buff[4];
    } dat1;

#if WORDS_BIGENDIAN == 1
    dat1.buff[0] = buffer[0];
    dat1.buff[1] = buffer[1];
    dat1.buff[2] = buffer[2];
    dat1.buff[3] = buffer[3];
#else 
    dat1.buff[0] = buffer[3];
    dat1.buff[1] = buffer[2];
    dat1.buff[2] = buffer[1];
    dat1.buff[3] = buffer[0];
#endif

    return dat1.f;
}

// STUB METHOD
float
ZTR_ConvertByteOrderedBufferTo16bitFloat( char* buffer )
{
    return (0.0);
}
