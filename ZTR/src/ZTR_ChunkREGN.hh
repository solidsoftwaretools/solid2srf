//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef ZTR_CHUNKREGN_HH
#define ZTR_CHUNKREGN_HH
//
#include <string>
#include <ZTR_Chunk.hh>

typedef enum
{
    ZTR_REGNtypesNone,
    ZTR_REGNtypesTech,
    ZTR_REGNtypesBio,
    ZTR_REGNtypesInverted,
    ZTR_REGNtypesDuplicate,
    ZTR_REGNtypesPaired,
    ZTR_REGNtypesExplicit,
    ZTR_REGNtypesNormal
} ZTR_REGNtypes;

class ZTR_ChunkREGN : public ZTR_Chunk
{
    public:
        ZTR_ChunkREGN( void );
        virtual ~ZTR_ChunkREGN( void );

        virtual bool initialize( std::istream& dataIn );

        bool setup( const std::vector<std::string>& namesIn,
                    const std::vector<ZTR_REGNtypes>& codesIn,
                    const std::vector<int>& boundaries,
                    ZTR_Type type = ZTR_Type32bitInt );

        virtual ZTR_ChunkType type( void ) const;

        virtual void dump( void ) const;

        const std::vector<std::string>& getNames( void ) const;
        const std::vector<ZTR_REGNtypes>& getCodes( void ) const;

    protected:
        ZTR_Type defaultDataType( void ) const;

    private:
        bool constructRegnNameList( std::string* regnNameList ) const;
        bool extractRegnNameList( const std::string& regnNameList );
        ZTR_REGNtypes charToCodes( char c ) const;
        char codesToChar( ZTR_REGNtypes type ) const;

        std::vector<std::string> names;
        std::vector<ZTR_REGNtypes> codes;
};

#endif
