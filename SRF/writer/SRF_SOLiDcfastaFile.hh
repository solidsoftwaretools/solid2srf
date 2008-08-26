//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef SRF_SOLIDCFASTAFILE_HH
#define SRF_SOLIDCFASTAFILE_HH
//

#include <SRF_SOLiDfile.hh>

class SRF_SOLiDcfastaFile : public SRF_SOLiDfile
{
    public:
        SRF_SOLiDcfastaFile( void );

        ~SRF_SOLiDcfastaFile( void );

        bool readNextBlock( SRF_SOLiDdataSet* dataSet );

    protected:

    private:
};

#endif
