//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef SRF_SOLIDWRITER_HH
#define SRF_SOLIDWRITER_HH
//
#include <string>
#include <ZTR_ChunkREGN.hh>
#include <SRF_File.hh>
#include <SRF_SOLiDstructs.hh>
#include <SRF_SOLiDfile.hh>
#include <SRF_SOLiDcfastaFile.hh>
#include <SRF_SOLiDqualFile.hh>
#include <SRF_SOLiDintensityFiles.hh>
#include <SRF_CommandLineArgs.hh>

typedef struct
{
    SRF_SOLiDcfastaFile cfastaFile;
    SRF_SOLiDqualFile qualFile;
    SRF_SOLiDintensityFiles intensityFiles;
} SRF_SOLiDfileSet;

class SRF_SOLiDWriter
{
    public:
        SRF_SOLiDWriter( const SRF_CommandLineArgs& argsIn );
        virtual ~SRF_SOLiDWriter( void );

        virtual bool open( void ) = 0;
        virtual bool moreData( bool& allFilesEnd ) = 0;
        virtual bool writeNextBlock( void ) = 0;
        bool writeHeaderAndXml( void );
        bool writeFooter( void );

    protected:

        const SRF_CommandLineArgs& args;
        SRF_File *outputFile;

        bool openOutput( void );
        bool createBaseChunk( const std::string& calls,
                              std::string& blob  );
        bool createCnf1Chunk( const std::vector<int>& confs,
                              std::string& blob  );
        bool createSampChunks(
                        const std::vector<SRF_IntensityData> intensityData,
                        std::string& blob );
        bool createRegnChunk( const std::vector<std::string>& names,
                              const std::vector<ZTR_REGNtypes>& codes,
                              const std::vector<int>& boundaries,
                              std::string& blob );

        virtual bool openFileSet( SRF_SOLiDfileSet* fileSet,
                                  const char* cfastaCode,
                                  const char* qualCode,
                                  const char* intensityCode );

        virtual bool moreData( SRF_SOLiDfileSet& fileSet,
                               bool& allFilesEnd );

        void generatePrefix( const SRF_SOLiDdataSet& dataset,
                             std::string* prefix ) const;
    private:

};

#endif
