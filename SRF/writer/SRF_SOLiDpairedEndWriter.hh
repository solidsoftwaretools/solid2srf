//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef SRF_SOLIDPAIREDENDWRITER_HH
#define SRF_SOLIDPAIREDENDWRITER_HH
//
#include <istream>
#include <SRF_SOLiDWriter.hh>
#include <SRF_SOLiDcfastaFile.hh>
#include <SRF_SOLiDqualFile.hh>
#include <SRF_SOLiDintensityFiles.hh>

class SRF_SOLiDpairedEndWriter : public SRF_SOLiDWriter
{
    public:
        SRF_SOLiDpairedEndWriter( const SRF_CommandLineArgs& argsIn );

        ~SRF_SOLiDpairedEndWriter( void );

        bool open( void );
        bool writeNextBlock( void );
        bool moreData( bool& allFilesEnd );

    protected:

    private:
        bool writeDBH( void );
        bool writeDB( void );
        bool identifyNextPairToWrite( void );
        void insertDummy( std::vector<SRF_SOLiDdataSet>& dummyInsertSet,
                          const std::vector<SRF_SOLiDdataSet>& goodSet,
                          char lastPrimerBase,
                          const std::string& suffix );
        bool createRegnChunk( std::string& blob );
        void modifyDataByREGNBlockContent( void );

       SRF_SOLiDfileSet fileSet1;
       SRF_SOLiDfileSet fileSet2;

       bool fileSet1HasMoreData;
       bool fileSet2HasMoreData;

       std::vector<SRF_SOLiDdataSet> dataSetsFile1;
       std::vector<SRF_SOLiDdataSet> dataSetsFile2;

       char lastPrimerBase1;
       char lastPrimerBase2;
       std::string suffix1;
       std::string suffix2;
};

#endif
