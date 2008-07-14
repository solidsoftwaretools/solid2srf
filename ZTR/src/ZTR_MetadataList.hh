//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef ZTR_METADATALIST_HH
#define ZTR_METADATALIST_HH
//
#include <string>
#include <map>
#include <ZTR_Types.hh>

class ZTR_MetadataList
{
    public:
        ZTR_MetadataList( void );

        ~ZTR_MetadataList( void );

        bool extract( const std::string& data );

        bool getMetadata( const std::string& key, std::string* value ) const;
        bool setMetadata( const std::string& key, const std::string& value );

        bool packForZTR( std::string* packedMetadata ) const;

        bool filter( const ZTR_MetadataList& filterAgainst ) const;

        ZTR_Type type( void ) const;

        void dump( void ) const;
    protected:

    private:

        std::map<std::string, std::string> metadata;
};

#endif
