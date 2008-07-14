//
#include <ios>
#include <iostream>
#include <string>
#include <stddef.h>
#include <SRF_util.hh>
#include <SRF_SOLiDWriter.hh>
#include <SRF_SOLiDsingleEndWriter.hh>
#include <SRF_SOLiDpairedEndWriter.hh>

int
main(int argc, char **argv)
{
    SRF_CommandLineArgs args;
    if ( argc < 5 || !args.extract( argc, argv ) )
    {
	std::cout << "Usage: writer -cf <cfasta file> -o <output file>\n"
                  << " -t <title>\n"
                  << " additional arguments optional\n"
                  << " -q <qual file>"
                  << " -i <intensities files root>\n"
                  << " -x <xml file>\n"
                  << " -to # -to overrides titles found in files is they\n"
                  << "     # don't match the title provided\n"
                  << " -noABprefix # removes AB_ prefix from readIds\n"
                  << " # The following flags are for the second set of files for paired end data\n"
                  << " # if a qual file is present for the first end, it must be present for the\n"
                  << " # second. Likewise for intensity files.\n"
                  << " -cf2 <second cfasta file>\n"
                  << " -q2 <second qual file>\n"
                  << " -i2 <second intensity files root>\n"
                  << " -noPrimerBaseInREGN\n";
	exit(1);
    }

    SRF_SOLiDWriter* writer = NULL;
    if ( args.isPairedEndWrite() )
    {
         writer = (SRF_SOLiDWriter*) new SRF_SOLiDpairedEndWriter( args );
    }
    else
    {
         writer = (SRF_SOLiDWriter*) new SRF_SOLiDsingleEndWriter( args );
    }
    
    if ( !writer->open() )
    {
        delete writer;
        exit( -1 );
    }

// write SRF container header
    if ( !writer->writeHeaderAndXml() )
    {
        std::cout << "ERROR: failed to write SRF header\n";
        delete writer;
        exit( -1 );
    }

    bool allFilesEnd = TRUE;
    while ( writer->moreData( allFilesEnd ) )
    {
        if ( !writer->writeNextBlock() )
        {
            std::cout << "ERROR: failed to write block\n";
            delete writer;
            exit(-1);
        }
    }

    if (!allFilesEnd)
    {
        std::cout << "ERROR: files have different content\n";
        delete writer;
        exit(-1);
    }

    if ( !writer->writeFooter() )
    {
        std::cout << "ERROR: failed to write SRF footer\n";
        delete writer;
        exit( -1 );
    }

    delete writer;

    exit( 0 );
}

