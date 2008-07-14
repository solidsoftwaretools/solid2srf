//Copyright 2007 Asim Siddiqui
//
// License TBD by AB

#ifndef SRF_ZTRREAD_HH
#define SRF_ZTRREAD_HH
//
#include <string>
#include <ZTR_ChunkList.hh>
#include <SRF_ZtrRead.hh>

class SRF_ZtrRead : public SRF_ZtrRead
{
    public:
        SRF_ZtrRead( void );

        ~SRF_ZtrRead( void );

        std::string& getBases( void ) const;
        const std::vector<int>& getQualities( void ) const;
        const std::vector<float>& getProcessedIntenstities( void ) const;

        std::string& getBases2of2( void ) const;
        const std::vector<int>& getQualities2of2( void ) const;
        const std::vector<float>& getProcessedIntenstities2of2( void ) const;

        bool isPairedEndRead( void ) const;

        bool extract( bool splitPair = FALSE );
       

    private:
        bool pairedEndRead;
        ZTR_ChunkList chunkList;

        std::string bases;
        ZTR_Data qualities;
        ZTR_Data processedIntensities;

        std::string bases2of2;
        ZTR_Data qualities2of2;
        ZTR_Data processedIntensities2of2;
};

#endif
