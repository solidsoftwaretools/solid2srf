//
#include <ios>
#include <vector>
#include <sstream>
#include <string>
#include <ZTR_ChunkBase.hh>

#define CSET_KEY "CSET"

ZTR_ChunkBase::ZTR_ChunkBase( void )
{
    cset = ZTR_ChunkBaseCharSetIUPAC;
}
    
ZTR_ChunkBase::~ZTR_ChunkBase( void )
{
    // no-op
}

ZTR_ChunkBaseCharSet
ZTR_ChunkBase::getCset( void ) const
{
    return cset;
}

ZTR_ChunkType
ZTR_ChunkBase::type( void ) const
{
    return ZTR_ChunkTypeBase;
}

ZTR_Type
ZTR_ChunkBase::defaultDataType( void ) const
{
    return ZTR_TypeString;
}

bool
ZTR_ChunkBase::initialize( std::istream& dataIn )
{
    if ( !ZTR_Chunk::initialize( dataIn ) )
    {
        ZTR_ReportError( "could not initialize chunk" );
        return FALSE;
    }
 
    std::string key( CSET_KEY );
    std::string value;
    if ( getMetadata()->getMetadata( key, &value ) )
    {
        if ( value == "I" )
        {
            cset = ZTR_ChunkBaseCharSetIUPAC;
        }
        else if ( value == "0" )
        {
            cset = ZTR_ChunkBaseCharSetColourSpace;
        }
        else
        {
            ZTR_ReportError( "could not initialize chunk" );
            return FALSE;
        }
    }

    return TRUE;
}

bool
ZTR_ChunkBase::setup( ZTR_ChunkBaseCharSet csetIn,
                      const std::string& calls )
{
    std::string key( CSET_KEY );
    std::string value;
    if ( csetIn == ZTR_ChunkBaseCharSetIUPAC )
    {
        value = "I";
    }
    else if ( csetIn == ZTR_ChunkBaseCharSetColourSpace )
    {
        value = "0";
    }
    else
    {
        ZTR_ReportError( "could not set up chunk" );
        return FALSE;
    }

    getMetadata()->setMetadata( key, value );

    setUncompressedDataStr( calls );    

    return TRUE;
}

void
ZTR_ChunkBase::dump( void ) const
{
    std::cout << "BASE CHUNK" << std::endl;
    ZTR_Chunk::dump();
    std::cout << "DATA: " << getUncompressedDataStr() << std::endl;
}
