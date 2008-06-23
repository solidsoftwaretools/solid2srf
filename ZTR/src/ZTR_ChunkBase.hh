//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef ZTR_CHUNKBASE_HH
#define ZTR_CHUNKBASE_HH
//
#include <string>
#include <ZTR_Chunk.hh>

typedef enum
{
    ZTR_ChunkBaseCharSetIUPAC,
    ZTR_ChunkBaseCharSetColourSpace,
} ZTR_ChunkBaseCharSet;


class ZTR_ChunkBase : public ZTR_Chunk
{
    public:
        ZTR_ChunkBase( void );
        virtual ~ZTR_ChunkBase( void );

        ZTR_ChunkBaseCharSet getCset( void ) const;

        virtual bool initialize( std::istream& dataIn );

        bool setup( ZTR_ChunkBaseCharSet csetIn,
                    const std::string& calls );

        virtual ZTR_ChunkType type( void ) const;

        virtual void dump( void ) const;

    protected:
        ZTR_Type defaultDataType( void ) const;

    private:
        ZTR_ChunkBaseCharSet cset;
};

#endif
