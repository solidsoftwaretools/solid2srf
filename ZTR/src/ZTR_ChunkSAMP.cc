//
#include <ios>
#include <vector>
#include <sstream>
#include <string>
#include <ZTR_util.hh>
#include <ZTR_ChunkSAMP.hh>

ZTR_ChunkSAMP::ZTR_ChunkSAMP( void )
{
    // no op
}
    
ZTR_ChunkSAMP::~ZTR_ChunkSAMP( void )
{
    // no-op
}

ZTR_ChunkType
ZTR_ChunkSAMP::type( void ) const
{
    return ZTR_ChunkTypeSAMP;
}

bool
ZTR_ChunkSAMP::chunkDataPadded( void ) const
{
    return TRUE;
}

ZTR_Type
ZTR_ChunkSAMP::defaultDataType( void ) const
{
    return ZTR_Type16bitInt;
}

bool
ZTR_ChunkSAMP::initialize( std::istream& dataIn )
{
    if ( !ZTR_Chunk::initialize( dataIn ) )
    {
        ZTR_ReportError( "could not initialize chunk" );
        return FALSE;
    }
 
    return TRUE;
}

bool
ZTR_ChunkSAMP::setup( const std::vector<int>& cnfIn, ZTR_Type type )
{
    std::string key( ZTR_METADATA_VALTYPE );
    std::string value;
    if ( type == ZTR_Type16bitInt )
    {
        value = ZTR_TYPE_16BIT_INT;
    }
    else if ( type == ZTR_Type32bitInt )
    {
        value = ZTR_TYPE_32BIT_INT;
    }
    else
    {
        ZTR_ReportError( "inappropriate type" );
        return FALSE;
    }

    getMetadata()->setMetadata( key, value );

    setUncompressedDataInt( cnfIn );    

    return TRUE;
}

bool
ZTR_ChunkSAMP::setup( const std::vector<float>& cnfIn, ZTR_Type type )
{
    std::string key( ZTR_METADATA_VALTYPE );
    std::string value;
    if ( type == ZTR_Type32bitFloat )
    {
        value = ZTR_TYPE_32BIT_FLOAT;
    }
    else
    {
        ZTR_ReportError( "inappropriate type" );
        return FALSE;
    }

    getMetadata()->setMetadata( key, value );

    setUncompressedDataFloat( cnfIn );

    return TRUE;
}

void
ZTR_ChunkSAMP::dump( void ) const
{
    std::cout << "SAMP CHUNK" << std::endl;
    ZTR_Chunk::dump();

    std::cout << "DATA:";
    if ( getDataType() == ZTR_Type16bitInt ||
         getDataType() == ZTR_Type32bitInt )
    {
        std::vector<int>::const_iterator it = getUncompressedDataInt().begin();

        while( it < getUncompressedDataInt().end() )
        {
            std::cout << " " << (*it);
            it++;
        }
        std::cout << std::endl;
    }
    else if ( getDataType() == ZTR_Type32bitFloat )
    {
        std::vector<float>::const_iterator it = getUncompressedDataFloat().begin();

        while( it < getUncompressedDataFloat().end() )
        {
            std::cout << " " << (*it);
            it++;
        }
        std::cout << std::endl;
    }

}

