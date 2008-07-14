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

int
main(int argc, char* argv[])
{
    if ( argc < 4 )
    {
        std::cout << "Usage: reader <input file> <output file>\n";
        std::cout << "<split paired end data> - TRUE or FALSE\n";
        std::cout << "Optional arguments:\n";
        std::cout << "<delete N chars from readId start> (0 is default)\n";
        std::cout << "<append seq name from REGN to read id> TRUE/FALSE\n";
        std::cout << " - set to TRUE for paired end data and FALSE for single end data\n";
        std::cout << "<chunk type> e.g. BASE (BASE is default)\n";
        std::cout << "KEY=VALUE e.g. TYPE=0FAM (default: 1st match returned\n";
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
    std::string pairedEndStr( argv[3] );
    bool pairedEndData = FALSE;
    SRF_FileOpenType outMode = SRF_FileOpenTypeWrite;
    if ( pairedEndStr == "TRUE" )
    {
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

    ZTR_ChunkMatchParams matchParams;
    matchParams.type = ZTR_ChunkTypeBase;
    bool appendReadId = FALSE;
    int cutReadId = 0;
    if ( argc > 4 )
    {
        cutReadId = atoi( argv[4] );
        if ( argc > 5 )
        {
            std::string appendReadIdStr( argv[5] );
            if ( appendReadIdStr == "TRUE" )
            {
                appendReadId = TRUE;
            }
        }
    }
    if ( argc > 6 )
    {
        std::string chunkType( argv[6] );
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

    if ( argc == 8 )
    {
        std::string keyValue( argv[7] ), key, value;

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

            ZTR_RetrieveChunk retrieveChunk;
            if ( !retrieveChunk.extract( chunkList,
                                         matchParams,
                                         pairedEndData ) )
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
                if ( pairedEndData )
                {
                    readId2 += '_' + retrieveChunk.getPairedEndName2();
                }
            }
            (output1->getFile()) << ">" << readId << std::endl;
            if ( pairedEndData )
            {
                (output2->getFile()) << ">" << readId2 << std::endl;
            }

            (output1->getFile()) << retrieveChunk.getDataForOutput();
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
