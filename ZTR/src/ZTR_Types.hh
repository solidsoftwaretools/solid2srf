//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef ZTR_TYPES_HH
#define ZTR_TYPES_HH
//
#include <string>

typedef enum
{
    ZTR_TypeNone,
    ZTR_TypeString,
    ZTR_Type8bitUnsignedInt,
    ZTR_Type16bitInt,
    ZTR_Type32bitInt,
    ZTR_Type16bitFloat,
    ZTR_Type32bitFloat
} ZTR_Type;

#define ZTR_TYPE_32BIT_FLOAT "32 bit IEEE float"
#define ZTR_TYPE_16BIT_INT "16 bit integer"
#define ZTR_TYPE_32BIT_INT "32 bit integer"

int ZTR_TypeNumberOfByte( ZTR_Type type );

int ZTR_ConvertByteOrderedBufferTo32bitInt( char* buffer );
int ZTR_ConvertByteOrderedBufferTo16bitInt( char* buffer );

float ZTR_ConvertByteOrderedBufferTo32bitFloat( char* buffer );
float ZTR_ConvertByteOrderedBufferTo16bitFloat( char* buffer );

#endif
