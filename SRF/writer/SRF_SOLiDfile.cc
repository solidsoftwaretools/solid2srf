//
#include <map>
#include <string>
#include <sstream>
#include <cstdio>
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

#define CHUNK 12288

SRF_SOLiDfile::SRF_SOLiDfile( void )
{
    fileOpen = FALSE;
}


SRF_SOLiDfile::~SRF_SOLiDfile( void )
{
  // Cleanup is causing segfaults. Need to debug.
  // For now, skip DELETE in destructor (program is exiting anyway)

#ifdef HAVE_EXT_STDIO_FILEBUF_H  
    if(pipe != NULL && fp != NULL) {
        pclose(fp); // Close underlying pipe fd
        //DELETE( pipe );
	fp = NULL;
    }
#endif
    if(file != NULL) {
	// // istream has no close()
        // if(std::ifstream *f2 = dynamic_cast<std::ifstream *>(file)) {
	//   // But if it's really a ifstream, we can close it.
	//   (*f2).close();
	// }
        //DELETE( file );	
    }
}

bool
SRF_SOLiDfile::open( const std::string& inputfilename,
                     const std::string& title,
                     bool titleOverride )
{
    filename = inputfilename;

    size_t found = filename.rfind(".gz");
    if (found != std::string::npos) {
#ifdef HAVE_EXT_STDIO_FILEBUF_H
        using __gnu_cxx::stdio_filebuf;
        std::string cmd = "gzip -dc " + filename;
        fp = popen(cmd.c_str(), "r");
        pipe = new stdio_filebuf<char>( fp, std::ios::in, CHUNK );
        file = new std::istream(pipe);
#else
        std::cerr << "ERROR: Unable to read gz file. Recompile with g++ extensions" << std::endl;
        return FALSE;
#endif // STDIO_FILEBUF_H
    } else {
        // This runs when zlib isn't enabled, or when the file isn't a gz file
        file = new std::ifstream( filename.c_str(), std::ifstream::in );
    }

    if ( !(*file).good() )
    {
        DELETE( file );
        std::cerr << "ERROR: Writer can't open input file"
                  << filename << std::endl;
        return FALSE;
    }

    std::string titleInFile = "";
    while( (*file).peek() == '#' )
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
    if ( titleInFile.empty() || titleInFile != title )
    {
	std::cerr << "WARNING: titles don't match '" << title << "' vs '"
		  << titleInFile << "'" << std::endl;
        if ( !titleOverride )
	{
            std::cerr << "ERROR: title mismatch. Override with -to argument"
                 << std::endl;
            return FALSE;
        }
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
        std::cerr << "ERROR: unexpected file format around readId: " <<
                cfastaPartialReadId << std::endl;
	std::string line;
	std::getline( (*file), line);
	std::cerr << "BAD READ: " << line << std::endl;
	std::cerr << "File: " << filename << std::endl;
        return FALSE;
    }

    char tmp;
    std::string partialReadId;
    (*file) >> tmp >> partialReadId;
    (*file).get(); // new line char
    if ( partialReadId != cfastaPartialReadId )
    {
        std::cerr << "ERROR: readIds don't match. CFASTA: " <<
                cfastaPartialReadId << std::endl
             << "QUAL: " << partialReadId << std::endl;
	std::cerr << "File: " << filename << std::endl;
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
