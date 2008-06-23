//
#include <ios>
#include <vector>
#include <sstream>
#include <string>
#include <ZTR_ChunkREGN.hh>

#define SCALE_KEY "SCALE"

ZTR_ChunkREGN::ZTR_ChunkREGN( void )
{
    // no-op
}
    
ZTR_ChunkREGN::~ZTR_ChunkREGN( void )
{
    // no-op
}

ZTR_ChunkType
ZTR_ChunkREGN::type( void ) const
{
    return ZTR_ChunkTypeREGN;
}

ZTR_Type
ZTR_ChunkREGN::defaultDataType( void ) const
{
    return ZTR_Type32bitInt;
}

bool
ZTR_ChunkREGN::initialize( std::istream& dataIn )
{
    if ( !ZTR_Chunk::initialize( dataIn ) )
    {
        ZTR_ReportError( "could not initialize chunk" );
        return FALSE;
    }
 
    names.clear();
    codes.clear();
    std::string key( ZTR_METADATA_NAME );
    std::string regnNameList;
    if ( getMetadata()->getMetadata( key, &regnNameList ) )
    {
        if ( !extractRegnNameList( regnNameList ) )
        {
            ZTR_ReportError( "could not extract name list" );
            return FALSE;
        }
    }

    return TRUE;
}

ZTR_REGNtypes
ZTR_ChunkREGN::charToCodes( char c ) const
{
    switch ( c )
    {
        case 'T':
            return ZTR_REGNtypesTech;
        case 'B':
            return ZTR_REGNtypesBio;
        case 'I':
            return ZTR_REGNtypesInverted;
        case 'D':
            return ZTR_REGNtypesDuplicate;
        case 'P':
            return ZTR_REGNtypesPaired;
        case 'E':
            return ZTR_REGNtypesExplicit;
        case 'N':
            return ZTR_REGNtypesNormal;
        default:
            return ZTR_REGNtypesNone;
    }
}

char
ZTR_ChunkREGN::codesToChar( ZTR_REGNtypes type ) const
{
    switch ( type )
    {
        case ZTR_REGNtypesTech:
            return 'T';
        case ZTR_REGNtypesBio:
            return 'B';
        case ZTR_REGNtypesInverted:
            return 'I';
        case ZTR_REGNtypesDuplicate:
            return 'D';
        case ZTR_REGNtypesPaired:
            return 'P';
        case ZTR_REGNtypesExplicit:
            return 'E';
        case ZTR_REGNtypesNormal:
            return 'N';
        default:
            return ' ';
    }
}

bool
ZTR_ChunkREGN::extractRegnNameList( const std::string& regnNameList )
{
    std::string::size_type i = 0;
    std::string::size_type j = regnNameList.find( ':' );

    while( j != std::string::npos )
    {
        names.push_back( regnNameList.substr( i, j - i ) );
        // j = ':', j+1= code, j+2 = ';'
        if ( regnNameList.length() < j+1 )
        {
            ZTR_ReportError( "invalid REGN metadata" );
            return FALSE;
        }

        ZTR_REGNtypes code = charToCodes( regnNameList[j+1] );
        if ( code == ZTR_REGNtypesNone )
        {
            ZTR_ReportError( "invalid REGN code" );
            return FALSE;
        }
        codes.push_back( code );

        if ( regnNameList.length() > (j + 3 ) )
        {
            j += 3;
            i = j;
            j = regnNameList.find( ':', i );
        }
        else
        {
            j = std::string::npos;
        }
    }

    return TRUE;
}

bool
ZTR_ChunkREGN::constructRegnNameList( std::string* regnNameList ) const
{
    if ( names.size() != codes.size() )
    {
        ZTR_ReportError( "names and codes are different lengths" );
        return FALSE;
    }

    int ii = 0;
    while ( ii < codes.size() )
    {
        *regnNameList += names[ii] + ':' + codesToChar( codes[ii] );
        ii++;
        if ( ii < codes.size() )
        {
            *regnNameList += ';';
        }
    }

    return TRUE;
}
    
bool
ZTR_ChunkREGN::setup( const std::vector<std::string>& namesIn,
                      const std::vector<ZTR_REGNtypes>& codesIn,
                      const std::vector<int>& boundaries,
                      ZTR_Type type )
{
    names = namesIn;
    codes = codesIn;
    std::string regnNameList;
    if ( !constructRegnNameList( &regnNameList ) )
    {
        ZTR_ReportError( "cannot construct REGN metadata" );
        return FALSE;
    }
    std::string regnNameListKey( ZTR_METADATA_NAME );
    getMetadata()->setMetadata( regnNameListKey, regnNameList );

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

    setUncompressedDataInt( boundaries );    

    return TRUE;
}

void
ZTR_ChunkREGN::dump( void ) const
{
    std::cout << "REGN CHUNK" << std::endl;
    ZTR_Chunk::dump();

    std::cout << "DATA:";
    std::vector<int>::const_iterator it = getUncompressedDataInt().begin();

    while( it < getUncompressedDataInt().end() )
    {
        std::cout << " " << (*it);
        it++;
    }

    std::cout << std::endl;
}

