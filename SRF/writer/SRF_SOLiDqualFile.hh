//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef SRF_SOLIDQUALFILE_HH
#define SRF_SOLIDQUALFILE_HH
//
#include <vector>
#include <string>
#include <SRF_SOLiDfile.hh>

class SRF_SOLiDqualFile : public SRF_SOLiDfile
{
    public:
        SRF_SOLiDqualFile( void );

        ~SRF_SOLiDqualFile( void );

        bool readNextBlock( const std::string& cfastaPartialReadId,
                            SRF_SOLiDdataSet* dataSet );

    protected:

    private:

};

#endif
