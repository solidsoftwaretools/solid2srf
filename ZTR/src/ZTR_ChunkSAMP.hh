//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef ZTR_CHUNKSAMP_HH
#define ZTR_CHUNKSAMP_HH
//
#include <string>
#include <ZTR_Chunk.hh>

class ZTR_ChunkSAMP : public ZTR_Chunk
{
    public:
        ZTR_ChunkSAMP( void );
        virtual ~ZTR_ChunkSAMP( void );

        virtual bool initialize( std::istream& dataIn );

        bool setup( const std::vector<int>& cnfIn, ZTR_Type type );
        bool setup( const std::vector<float>& cnfIn,
                    ZTR_Type type = ZTR_Type32bitFloat );

        virtual ZTR_ChunkType type( void ) const;

        virtual void dump( void ) const;

    protected:
        ZTR_Type defaultDataType( void ) const;

    private:
};

#endif
