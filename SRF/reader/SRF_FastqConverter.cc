//
#include <ios>
#include <stddef.h>
#include <ZTR_RetrieveChunk.hh>
#include <ZTR_ChunkList.hh>
#include <SRF_File.hh>
#include <SRF_Block.hh>
#include <SRF_ContainerHeader.hh>
#include <SRF_DataBlockHeader.hh>
#include <SRF_Read.hh>

#include <stdio.h>
#include <string.h>
#include <cstring>
#include <stdlib.h>

//g++ -DHAVE_CONFIG_H -I. -I. -I../.. -I../../SRF/base -I../../ZTR/src    -g -O2 -Wall -Wstrict-aliasing=2  -MT SRF_FastqConverter.o -MD -MP -MF ".deps/SRF_FastqConverter.Tpo" -c -o SRF_FastqConverter.o SRF_FastqConverter.cc;
//g++  -g -O2 -Wall -Wstrict-aliasing=2    -o SRF_FastqConverter  SRF_FastqConverter.o ../../SRF/base/libsrf.a  ../../ZTR/src/libztr.a  -lstdc++ -lm

int
main(int argc, char* argv[])
{
    if ( argc < 3 )
    {
        std::cout << "Usage: reader <input file> <output file>\n";
        //std::cout << "<split paired end data> - TRUE or FALSE\n";
        std::cout << "Optional arguments:\n";
        std::cout << "<delete N chars from readId start> (0 is default)\n";
        std::cout << "<append seq name from REGN to read id> TRUE/FALSE\n";
        std::cout << " - set to TRUE for paired end data and FALSE for single end data\n";
        //std::cout << "<chunk type> e.g. BASE (BASE is default)\n";
        //std::cout << "KEY=VALUE e.g. TYPE=0FAM (default: 1st match returned\n";
        exit(1);
    }

    SRF_FileOpenType mode = SRF_FileOpenTypeRead;
    SRF_File file( argv[1], mode );
    if ( !file.open() )
    {
        std::cout << "ERROR: Unable to open input file\n";
        exit(1);
    }

    std::string outputFileBase( argv[2] );
    SRF_File* output1 = NULL;
    SRF_File* output2 = NULL;
    std::string pairedEndStr( "FALSE" ); //argv[3] );
    bool pairedEndData = FALSE;
    SRF_FileOpenType outMode = SRF_FileOpenTypeWrite;
    if ( pairedEndStr == "TRUE" )
    {
        std::cerr << "Not implemented yet." << std::endl;
        exit(1);
        pairedEndData = TRUE;
        std::string out1 = outputFileBase + ".1";
        std::string out2 = outputFileBase + ".2";
        output1 = new SRF_File( out1.c_str(), outMode );
        output2 = new SRF_File( out2.c_str(), outMode );

        if (!output1->open() )
        {
            std::cout << "ERROR: Writer can't open output file" <<
                         out1 << std::endl;
            exit( -1 );
        }

        if (!output2->open() )
        {
            std::cout << "ERROR: Writer can't open output file" <<
                         out2 << std::endl;
            exit( -1 );
        }
    }
    else
    {
        output1 = new SRF_File( outputFileBase.c_str(), outMode );

        if (!output1->open() )
        {
            std::cout << "ERROR: Writer can't open output file" << 
                         outputFileBase << std::endl;
            exit( -1 );
        }
    }

    ZTR_ChunkMatchParams matchParams, matchParams2;
    matchParams.type = ZTR_ChunkTypeBase;
    matchParams2.type = ZTR_ChunkTypeCNF1;
    bool appendReadId = FALSE;
    int cutReadId = 0;
    if ( argc > 4 )
    {
        cutReadId = atoi( argv[3] );
        if ( argc > 5 )
        {
            std::string appendReadIdStr( argv[4] );
            if ( appendReadIdStr == "TRUE" )
            {
                appendReadId = TRUE;
            }
        }
    }

    /*
    if ( argc > 6 )
    {
        std::string chunkType( argv[5] );
        if ( chunkType == "BASE" )
        {
            matchParams.type = ZTR_ChunkTypeBase;
        }
        else if ( chunkType == "CNF1" )
        {
            matchParams.type = ZTR_ChunkTypeCNF1;
        }
        else if ( chunkType == "SAMP" )
        {
            matchParams.type = ZTR_ChunkTypeSAMP;
        }
        else if ( chunkType == "REGN" )
        {
            matchParams.type = ZTR_ChunkTypeREGN;
        }
        else
        {
            std::cout << "ERROR: Unknown chunk type: must be one of BASE, CNF1, SAMP or REGN" <<std::endl;
            exit( -1 );
        }
    }
    */

    if ( argc == 8 )
    {
        std::string keyValue( argv[5] ), key, value;

        int pos = keyValue.find('=');
        key.assign( keyValue, 0, pos );
        value.assign( keyValue, (pos+1), keyValue.size() );

        matchParams.metadata.setMetadata( key, value );
    }
    
    SRF_ContainerHeader* currContainer = NULL;
    SRF_DataBlockHeader* currDataBlockHeader = NULL;

    while ( file.getFile().peek() != EOF )
    {
        SRF_Block* block = NULL;
        SRF_BlockType blockType = SRF_GetNewBlock( file.getFile(),
                                                   ADOPT &block );
        if ( blockType == SRF_BlockTypeNone )
        {
            std::cout << "ERROR: Unable to read block\n";
            exit(1);
        }

        if ( blockType == SRF_BlockTypeContainerHeader )
        {
            if ( currContainer != NULL )
            {
                delete currContainer;
            }
            currContainer = (SRF_ContainerHeader*) block;
        }
        else if ( blockType == SRF_BlockTypeDataBlockHeader )
        {
            if ( currDataBlockHeader != NULL )
            {
                delete currDataBlockHeader;
            }
            currDataBlockHeader = (SRF_DataBlockHeader*) block;
        }
        else if ( blockType == SRF_BlockTypeDataBlock )
        {
            SRF_Read read( currContainer, currDataBlockHeader,
                              ABANDON (SRF_DataBlock*) block );

            const std::string& ztrBlob = read.getData();
            ZTR_ChunkList chunkList;
            if ( !chunkList.extract( ztrBlob ) )
            {
                std::cout << "ERROR: Unable to read blob\n";
                exit(1);
            }

            ZTR_RetrieveChunk retrieveChunk, retrieveChunk2;
	    retrieveChunk2.setUseFastQ();

	    bool c1Return = !retrieveChunk.extract( chunkList,
						    matchParams,
						    pairedEndData );
            if ( !retrieveChunk2.extract( chunkList,
					  matchParams2,
					  pairedEndData ) || c1Return )
            {
                std::cout << "ERROR: Unable to find requested data\n";
                exit(1);
            }

            std::string readId = read.getReadId();
            if ( cutReadId > 0 )
            {
                readId.erase( 0, cutReadId );
            }
            std::string readId2 = readId;
            if ( appendReadId )
            {
                readId += '_' + retrieveChunk.getPairedEndName1();
            }
            (output1->getFile()) << "@" << readId << std::endl;
            (output1->getFile()) << retrieveChunk.getDataForOutput();
	    (output1->getFile()) << "+" << std::endl;
	    (output1->getFile()) << retrieveChunk2.getDataForOutput();
            if ( pairedEndData )
            {
                (output2->getFile())
                    << retrieveChunk.getData2of2ForOutput();
            }
        }
        else if ( blockType == SRF_BlockTypeNullIndex )
        {
        }
        else
        {
            std::cout << "ERROR: Unknown block type\n";
            exit(1);
        }
    }

    delete currContainer;
    delete currDataBlockHeader;
    DELETE( output1 );
    DELETE( output2 );
}
