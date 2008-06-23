//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef ZTR_DATACOMPRESSIONRAW_HH
#define ZTR_DATACOMPRESSIONRAW_HH
//
#include <string>
#include <ZTR_util.hh>
#include <ZTR_DataCompression.hh>

class ZTR_Chunk;

class ZTR_DataCompressionRaw : public ZTR_DataCompression
{
    public:
        ZTR_DataCompressionRaw( void );
        ~ZTR_DataCompressionRaw( void );

        virtual bool byteOrderAndUncompress(
                                    const std::string& compressedData,
                                    ZTR_Chunk *theChunk ) const;
        virtual bool compressAndByteOrder(
                                    const ZTR_Chunk* theChunk,
                                    std::string* compressedData ) const;

        virtual ZTR_DataCompressionType getDataCompressionType( void ) const;
    protected:

    private:

};

#endif
