//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef SRF_SOLIDFILE_HH
#define SRF_SOLIDFILE_HH
//
#include <istream>
#include <fstream>
#include <string>

class SRF_SOLiDfile
{
    public:
        SRF_SOLiDfile( void );

        virtual ~SRF_SOLiDfile( void );

        virtual bool open( const std::string& filename,
                           const std::string& title,
                           bool titleOverride );

        virtual bool moreData( void );

        virtual bool readNextBlock( const std::string& cfastaPartialReadId );

        virtual bool isFileOpen( void ) const;

    protected:
        bool readNextBlockCommon( const std::string& cfastaPartialReadId );

        std::ifstream file;

        // FRIEND CLASS
        friend class SRF_SOLiDintensityFiles;

    private:
        bool fileOpen;
};

#endif
