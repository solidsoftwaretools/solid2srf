//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef ZTR_DATACOMPRESSION_HH
#define ZTR_DATACOMPRESSION_HH
//
#include <string>
#include <ZTR_util.hh>
#include <ZTR_Types.hh>

class ZTR_Chunk;

typedef enum
{
     ZTR_DataCompressionTypeUnspecified,
     ZTR_DataCompressionTypeRaw
} ZTR_DataCompressionType;

class ZTR_DataCompression
{
    public:
        ZTR_DataCompression( void );
        virtual ~ZTR_DataCompression( void );

        virtual bool byteOrderAndUncompress(
                                    const std::string& compressedData,
                                    ZTR_Chunk *theChunk ) const = 0;
        virtual bool compressAndByteOrder(
                                    const ZTR_Chunk* theChunk,
                                    std::string* compressedData ) const = 0;

        virtual ZTR_DataCompressionType getDataCompressionType( void ) const;

    protected:

    private:

};

ADOPT ZTR_DataCompression*
ZTR_GetNewDataCompression( int format );

int
ZTR_DataCompressionTypeToFormat( ZTR_DataCompressionType type );

#endif
