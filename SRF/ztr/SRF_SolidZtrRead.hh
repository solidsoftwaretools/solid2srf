//Copyright 2007 Asim Siddiquifor AB
//
// License TBD by AB

#ifndef SRF_SOLIDZTRREAD_HH
#define SRF_SOLIDZTRREAD_HH
//
#include <string>
#include <SRF_ZtrRead.hh>

class SRF_SolidZtrRead : public SRF_ZtrRead
{
    public:
        SRF_SolidZtrRead( void );

        ~SRF_SolidZtrRead( void );

        std::vector<SRF_Data> getRawIntenstities( void ) const;

    private:
};

#endif
