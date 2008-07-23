//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef SRF_SOLIDINTENSITYFILES_HH
#define SRF_SOLIDINTENSITYFILES_HH
//
#include <iosfwd>
#include <vector>
#include <string>
#include <SRF_SOLiDfile.hh>
#include <SRF_SOLiDstructs.hh>

class SRF_SOLiDintensityFiles
{
    public:
        SRF_SOLiDintensityFiles( void );

        ~SRF_SOLiDintensityFiles( void );

        bool open( const std::string& intensityFilesRoot,
                   const std::string& title,
                   bool titleOverride );
        bool readNextBlock( const std::string& cfastaPartialReadId,
                            SRF_SOLiDdataSet* dataSet );
        bool moreData( void );
        bool areFilesOpen( void ) const;

    protected:

    private:
       bool readIntensityValues( SRF_SOLiDfile& file,
                                 std::vector<float>& values );

       void checkExtension( std::string& fileStr );

        SRF_SOLiDfile ftcIntens;
        SRF_SOLiDfile cy3Intens;
        SRF_SOLiDfile txrIntens;
        SRF_SOLiDfile cy5Intens;

};

#endif
