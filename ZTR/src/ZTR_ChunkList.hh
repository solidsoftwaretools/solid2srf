//Copyright 2008 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef ZTR_CHUNKLIST_HH
#define ZTR_CHUNKLIST_HH
//
#include <string>
#include <map>
#include <ZTR_Chunk.hh>

class ZTR_ChunkList
{
    public:
        ZTR_ChunkList( void );

        ~ZTR_ChunkList( void );

        bool extract( const std::string& data );

        bool addChunk( ABANDON ZTR_Chunk* chunk);
        bool getChunk( int pos, const ZTR_Chunk* chunk ) const;
        int numChunks( void ) const;

        bool packForZTR( std::string* packedBlob ) const;

        void dump( void ) const;

    protected:

    private:

        std::vector<ZTR_Chunk*> chunks;

        void deleteChunks();
};

#endif
