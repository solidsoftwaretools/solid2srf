//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef SRF_SOLIDFILE_HH
#define SRF_SOLIDFILE_HH
//
#include <iosfwd>
#include <string>

#ifdef HAVE_CONFIG_H
#include <srf_config.h>
#endif

#ifdef HAVE_EXT_STDIO_FILEBUF_H
#include <ext/stdio_filebuf.h>
#include <cstdio>
#endif


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

        std::istream* file;
#ifdef HAVE_EXT_STDIO_FILEBUF_H
        __gnu_cxx::stdio_filebuf<char>* pipe;
        FILE* fp;
#endif

        // FRIEND CLASS
        friend class SRF_SOLiDintensityFiles;

    private:
        bool fileOpen;
};

#endif
