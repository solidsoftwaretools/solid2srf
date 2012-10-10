//
#include <map>
#include <string>
#include <sstream>
#include <cstdio>
#include <ZTR_ChunkList.hh>
#include <ZTR_util.hh>

#define ZTR_MAGIC_NUMBER "\256ZTR\r\n\032\n"
#define ZTR_VERSION_1_3 1.30001 // avoid rounding errors
#define ZTR_VERSION_2_0 2.0
#define ZTR_VERSION_1_4 1.40001
#define ZTR_VERSION_MAJOR 2
#define ZTR_VERSION_MINOR 0

ZTR_ChunkList::ZTR_ChunkList( void )
{
    // no-op
}
    
ZTR_ChunkList::~ZTR_ChunkList( void )
{
    deleteChunks();
}

void
ZTR_ChunkList::deleteChunks( void )
{
    std::vector<ZTR_Chunk*>::iterator iter = chunks.begin();
    while ( iter != chunks.end() )
    {
        DELETE( *iter );
        iter++;
    }

    chunks.clear();
}

bool
ZTR_ChunkList::extract( const std::string& data )
{
    std::istringstream inputData( data );

    std::string header;
    char* buffer = new char[8];
    inputData.read( buffer, 8 );
    header.assign( buffer, 8 );
    DELETE_ARRAY( buffer );

    if ( header != ZTR_MAGIC_NUMBER )
    {
        ZTR_ReportError( "data has incorrect ZTR header" );
        return FALSE;
    }

    uint8_t major, minor;
    inputData.read( reinterpret_cast<char *>(&major), 1 );
    inputData.read( reinterpret_cast<char *>(&minor), 1 );

    float version = ( (float) major ) + ( ((float) minor) / 10.0 );
    if ( version > ZTR_VERSION_1_4 )
    {
        ZTR_ReportError( "ZTR Version not supported" );
        return FALSE;
    }

    deleteChunks();

    while ( inputData.peek() != EOF )
    {
        ZTR_Chunk* newChunk = NULL;
        ZTR_ChunkType chunkType = ZTR_GetNewChunk( inputData, ADOPT &newChunk );
        if ( chunkType == ZTR_ChunkTypeNone )
        {
            ZTR_ReportError( "could not read ZTR chunk" );
            return FALSE;
        }

        chunks.push_back( ABANDON newChunk );
    }

    return TRUE;
}

bool
ZTR_ChunkList::getChunk( unsigned int pos,
                         const ZTR_Chunk* chunk ) const
{
    if ( pos >= chunks.size() )
    {
        ZTR_ReportError( "requested chunk beyond list end" );
        return FALSE;
    }
    chunk = chunks[pos];

    return TRUE;
}

bool
ZTR_ChunkList::addChunk( ABANDON ZTR_Chunk* chunk)
{
    chunks.push_back( ABANDON chunk );

    return TRUE;
}

bool
ZTR_ChunkList::packForZTR( std::string* packedBlob ) const
{
    *packedBlob = ZTR_Header();
    
    std::vector<ZTR_Chunk*>::const_iterator it = chunks.begin();

    while( it != chunks.end() )
    {
        (*packedBlob) += (*it)->getData();

        it++;
    }

    return TRUE;
}

void
ZTR_ChunkList::dump( void ) const
{
    std::vector<ZTR_Chunk*>::const_iterator it = chunks.begin();

    while( it != chunks.end() )
    {
        (*it)->dump();

        it++;
    }
}

int
ZTR_ChunkList::numChunks( void ) const
{
    return chunks.size();
}

const std::vector<ZTR_Chunk*>&
ZTR_ChunkList::getChunks( void ) const
{
    return chunks;
}

// TODO add method to allow SRF header body split
