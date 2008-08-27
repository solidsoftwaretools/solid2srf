//Copyright 2007 Asim Siddiqui
//
// License TBD by AB

#ifndef SRF_ZTRREAD_HH
#define SRF_ZTRREAD_HH
//
#include <string>
#include <ZTR_Chunk.hh>
#include <ZTR_ChunkList.hh>
#include <ZTR_MetadataList.hh>
#include <ZTR_RetrieveChunk.hh>

class ZTR_ChunkREGN;

typedef struct
{
    ZTR_ChunkType type;
    ZTR_MetadataList metadata;
} ZTR_ChunkMatchParams;

class ZTR_RetrieveChunk
{
    public:
        ZTR_RetrieveChunk( void );

        ~ZTR_RetrieveChunk( void );

        const ZTR_Data& getData( void ) const;
        const ZTR_Data& getData2of2( void ) const;

        bool isPairedEndRead( void ) const;
        void setUseFastQ ();

        bool extract( const ZTR_ChunkList& chunkList,
                      const ZTR_ChunkMatchParams& matchParams,
                      bool splitPair = FALSE );

        std::string getDataForOutput( void ) const;
        std::string getData2of2ForOutput( void ) const;

        const std::string& getPairedEndName1( void ) const;
        const std::string& getPairedEndName2( void ) const;

    private:
        bool matchChunk( const ZTR_Chunk* chunk,
                         const ZTR_ChunkMatchParams& matchParams );
        bool extractData( const ZTR_ChunkMatchParams& matchParams,
                          bool splitPair );

        void streamFastaQ ( const ZTR_Data& dataIn,
                            std::ostringstream &oss ) const;
        std::string getDataForOutputGen( const ZTR_Data& dataIn ) const;

        bool pairedEndRead;

        const ZTR_Chunk* matchedChunk;
        const ZTR_ChunkREGN* regnChunk;

        ZTR_Data data;
        ZTR_Data data2of2;

        std::string pairedEndName1;
        std::string pairedEndName2;

        bool useFastQ; //use fastZ format when printing out int
};

#endif
