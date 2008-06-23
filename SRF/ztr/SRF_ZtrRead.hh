//Copyright 2007 Asim Siddiqui
//
// License TBD by AB

#ifndef SRF_ZTRREAD_HH
#define SRF_ZTRREAD_HH
//
#include <string>
#include <SRF_Read.hh>

class SRF_ZtrRead : public SRF_Read
{
    public:
        SRF_ZtrRead( void );

        ~SRF_ZtrRead( void );

        std::string& getBases( void ) const;
        std::vector<SRF_Data> getQuality( void ) const;
        
        std::vector<SRF_Data> getProcessedIntenstities( void ) const;
        std::vector<SRF_Data> getRawIntenstities( void ) const;

    private:
};

#endif
