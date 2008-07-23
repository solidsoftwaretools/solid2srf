//
#include <map>
#include <string>
#include <sstream>
#include <SRF_util.hh>
#include <ZTR_util.hh>
#include <SRF_SOLiDfile.hh>

#ifdef HAVE_CONFIG_H
#include <srf_config.h>
#endif

#ifdef HAVE_EXT_STDIO_FILEBUF_H
#include <ext/stdio_filebuf.h>
#include <cstdio>
#endif

SRF_SOLiDfile::SRF_SOLiDfile( void )
{
    fileOpen = FALSE;
}


SRF_SOLiDfile::~SRF_SOLiDfile( void )
{
#ifdef HAVE_EXT_STDIO_FILEBUF_H  
    if(pipe != NULL) {
        (*pipe).close();
        delete pipe;
    }
    pclose(fp);
#endif
    if(file != NULL) {
        // istream has no close()
        delete file;
    }
}

bool
SRF_SOLiDfile::open( const std::string& filename,
                     const std::string& title,
                     bool titleOverride )
{

    size_t found = filename.rfind(".gz");
    if (found != std::string::npos) {
#ifdef HAVE_EXT_STDIO_FILEBUF_H
        using __gnu_cxx::stdio_filebuf;

        std::string cmd = "gzip -dc " + filename;
        fp = popen(cmd.c_str(), "r");
        pipe = new stdio_filebuf<char>( fp, std::ios::in );
        file = new std::istream(pipe);
#else
        std::cout << "ERROR: Unable to read gz file. Recompile with g++ extensions" << std::endl;
        return FALSE;
#endif // STDIO_FILEBUF_H
    } else {
        // This runs when zlib isn't enabled, or when the file isn't a gz file
        file = new std::ifstream( filename.c_str(), std::ifstream::in );
    }
    assert(file != NULL);

    if (!(*file).good() )
    {
        std::cout << "ERROR: Writer can't open input file: "
                  << filename << std::endl;
        return FALSE;
    }

    std::string titleInFile = "";
    while( (*file).peek() != '>' )
    {
        // if present, the title line has the following form
        // # Title: <title>
        std::string line, tmp1, tmp2, titleRead, tmp3;
        std::getline( (*file), line );
        std::istringstream lineStream( line );
        lineStream >> tmp1 >> std::skipws >> tmp2 >> std::skipws
                   >> titleRead >> tmp3;
        if ( tmp2 == "Title:" )
        {
            titleInFile = titleRead;
        }
    }

    // titles match?
    if ( titleInFile != title )
    {
        if ( !titleOverride )
        {
            std::cout << "ERROR: titles don't match " << title << " "
                      << titleInFile << std::endl;
            std::cout << "Override with -to argument"
                 << std::endl;
            return FALSE;
        }

        std::cout << "WARNING: titles don't match " << title << " "
                  << titleInFile << std::endl;
    }

    fileOpen = TRUE;
    return TRUE;
}

bool
SRF_SOLiDfile::moreData( void )
{
    if ( fileOpen && (*file).peek() == EOF )
    {
        return FALSE;
    }

    return TRUE;
}

bool
SRF_SOLiDfile::readNextBlockCommon( const std::string& cfastaPartialReadId )
{
    if ( !fileOpen )
    {
        return TRUE;
    }
    if ( (*file).peek() != '>' )
    {
        std::cout << "ERROR: unexpected file format around readId: " <<
                cfastaPartialReadId << std::endl;
        return FALSE;
    }

    char tmp;
    std::string partialReadId;
    *file >> tmp >> partialReadId;
    (*file).get(); // new line char
    if ( partialReadId != cfastaPartialReadId )
    {
        std::cout << "ERROR: readIds don't match. CFASTA: " <<
                cfastaPartialReadId << std::endl
             << "QUAL: " << partialReadId << std::endl;
        return FALSE;
    }

    return TRUE;
}

bool
SRF_SOLiDfile::isFileOpen( void ) const
{
    return fileOpen;
}

bool
SRF_SOLiDfile::readNextBlock( const std::string& cfastaPartialReadId )
{
    return FALSE;
}
