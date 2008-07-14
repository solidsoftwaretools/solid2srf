//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef ZTR_CHUNK_HH
#define ZTR_CHUNK_HH
//
#include <string>
#include <ZTR_util.hh>
#include <ZTR_Types.hh>
#include <ZTR_DataCompression.hh>
#include <ZTR_MetadataList.hh>

typedef enum
{
    ZTR_ChunkTypeNone,
    ZTR_ChunkTypeBase,
    ZTR_ChunkTypeCNF1,
    ZTR_ChunkTypeSAMP,
    ZTR_ChunkTypeREGN
} ZTR_ChunkType;

class ZTR_Chunk
{
    public:
        ZTR_Chunk( void );
        virtual ~ZTR_Chunk( void );

        virtual bool initialize( std::istream& dataIn );

        bool readyData( const ZTR_DataCompression* compressionScheme );
        const std::string& getData( void ) const;

        ZTR_Type getDataType( void ) const;

        ZTR_MetadataList* getMetadata( void );
        const ZTR_MetadataList* getMetadataConst( void ) const;

       void setUncompressedDataStr( const std::string& dataStrIn );
       void setUncompressedDataInt( const std::vector<int>& dataIntIn );
       void setUncompressedDataFloat( const std::vector<float>& dataFloatIn );

       const std::string& getUncompressedDataStr( void ) const;
       const std::vector<int>& getUncompressedDataInt( void ) const;
       const std::vector<float>& getUncompressedDataFloat( void ) const;

       const ZTR_Data& getAssignedData( void ) const;

       virtual ZTR_ChunkType type( void ) const;
       virtual bool chunkDataPadded( void ) const;

       virtual void dump( void ) const;

    protected:
        virtual ZTR_Type defaultDataType( void ) const;

    private:

        ZTR_MetadataList metadata;
        std::string data; // full chunk, type, metadata and format byte included
                          // not set until readyData called

        // one of these three vectors is filled with data
        ZTR_Data assignedData;
};

ZTR_ChunkType
ZTR_GetNewChunk( std::istream& data, ADOPT ZTR_Chunk** newChunk );

#endif
