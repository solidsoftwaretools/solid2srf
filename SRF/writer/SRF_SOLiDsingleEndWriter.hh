//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef SRF_SOLIDSINGLEENDWRITER_HH
#define SRF_SOLIDSINGLEENDWRITER_HH
//
#include <istream>
#include <SRF_SOLiDWriter.hh>
#include <SRF_SOLiDcfastaFile.hh>
#include <SRF_SOLiDqualFile.hh>
#include <SRF_SOLiDintensityFiles.hh>

class SRF_SOLiDsingleEndWriter : public SRF_SOLiDWriter
{
    public:
        SRF_SOLiDsingleEndWriter( const SRF_CommandLineArgs& argsIn );

        ~SRF_SOLiDsingleEndWriter( void );

        bool open( void );
        bool writeNextBlock( void );
        bool moreData( bool& allFilesEnd );

    protected:

    private:
        bool writeDBH( void );
        bool writeDB( void );
        bool createRegnChunk( std::string& blob );
        void modifyDataByREGNBlockContent( void );

        SRF_SOLiDfileSet fileSet;
        SRF_SOLiDdataSet currDataSet;
        SRF_SOLiDdataSet prevDataSet;
};

#endif
