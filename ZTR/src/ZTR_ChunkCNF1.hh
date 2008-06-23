//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef ZTR_CHUNKCNF1_HH
#define ZTR_CHUNKCNF1_HH
//
#include <string>
#include <ZTR_Chunk.hh>

typedef enum
{
    ZTR_ChunkCNFscalePHRED,
    ZTR_ChunkCNFscaleLO
} ZTR_ChunkCNFscale;

class ZTR_ChunkCNF1 : public ZTR_Chunk
{
    public:
        ZTR_ChunkCNF1( void );
        virtual ~ZTR_ChunkCNF1( void );

        ZTR_ChunkCNFscale getScale( void ) const;

        virtual bool initialize( std::istream& dataIn );

        bool setup( ZTR_ChunkCNFscale scaleIn,
                    const std::vector<int>& cnfIn );

        virtual ZTR_ChunkType type( void ) const;

        virtual void dump( void ) const;

    protected:
        ZTR_Type defaultDataType( void ) const;

    private:
        ZTR_ChunkCNFscale scale;
};

#endif
