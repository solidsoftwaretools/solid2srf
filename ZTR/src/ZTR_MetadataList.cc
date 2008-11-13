//
#include <iostream>
#include <map>
#include <string>

#include "ZTR_util.hh"
#include "ZTR_MetadataList.hh"

ZTR_MetadataList::ZTR_MetadataList( void )
{
    // no-op
}
    
ZTR_MetadataList::~ZTR_MetadataList( void )
{
    // no-op
}


bool
ZTR_MetadataList::extract( const std::string& data )
{
    if ( data.length() == 0 )
    {
        return TRUE;
    }

    char delim = '\000';

    std::string::size_type i = 0;
    std::string::size_type j = data.find( delim );
    std::vector<std::string> v;

    while( j != std::string::npos )
    {
        v.push_back( data.substr( i, j - i) );

        i = ++j;
        j = data.find( delim, j );

    }

    if ( v.size() % 2 != 0 )
    {
        ZTR_ReportError( "unexpected number of elements in metadata" );
        return FALSE;
    }

    metadata.clear();
    unsigned int ii = 0;
    while ( ii < v.size() )
    {
        metadata.insert( std::pair<std::string, std::string>(v[ii], v[ii+1] ));
        ii += 2;
    }

    return TRUE;
}

bool
ZTR_MetadataList::getMetadata( const std::string& key,
                               std::string* value ) const
{
    std::map<std::string, std::string>::const_iterator it;

    it = metadata.find( key );

    if ( it == metadata.end() )
    {
        // this is not a fatal error.
        *value = "";
        return FALSE;
    }

    *value = (*it).second;

    return TRUE;
}

bool
ZTR_MetadataList::setMetadata( const std::string& key,
                               const std::string& value )
{
    metadata.erase( key );
    metadata.insert( std::pair<std::string, std::string>(key, value) );

    return TRUE;
}

bool
ZTR_MetadataList::packForZTR( std::string* packedMetadata ) const
{
    
    std::map<std::string, std::string>::const_iterator it;
    it = metadata.begin();

    packedMetadata->clear();
    while( it != metadata.end() )
    {
        (*packedMetadata) = (*packedMetadata) +  (*it).first + '\000' +
                          (*it).second + '\000';

        it++;
    }

    return TRUE;
}

void
ZTR_MetadataList::dump( void ) const
{
    std::map<std::string, std::string>::const_iterator it;
    it = metadata.begin();

    std::cout << "METADATA" << std::endl;

    while( it != metadata.end() )
    {
        std::cout << (*it).first << " " << (*it).second << std::endl;

        it++;
    }
}


ZTR_Type
ZTR_MetadataList::type( void ) const
{
    std::string valType;
    if ( getMetadata( ZTR_METADATA_VALTYPE, &valType ) )
    {
        if ( valType == ZTR_TYPE_32BIT_FLOAT )
        {
            return ZTR_Type32bitFloat;
        }
        else if ( valType == ZTR_TYPE_16BIT_INT )
        {
            return ZTR_Type16bitInt;
        }
        else if ( valType == ZTR_TYPE_32BIT_INT )
        {
            return ZTR_Type32bitInt;
        }

    } 
    return ZTR_TypeNone;
}

bool
ZTR_MetadataList::filter( const ZTR_MetadataList& filterAgainst ) const
{
    std::map<std::string, std::string>::const_iterator it;
    it = metadata.begin();

    while( it != metadata.end() )
    {
        std::string keyIn = (*it).first;
        std::string valueIn = (*it).second;
        std::string valueOut;

        if ( !filterAgainst.getMetadata( keyIn, &valueOut ) )
        {
            return FALSE;
        }
        if ( valueIn != valueOut )
        {
            return FALSE;
        }

        it++;
    }

    return TRUE;
}
