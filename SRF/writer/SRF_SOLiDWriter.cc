//
#include <iostream>
#include <map>
#include <string>
#include <ZTR_util.hh>
#include <ZTR_Chunk.hh>
#include <ZTR_ChunkBase.hh>
#include <ZTR_ChunkCNF1.hh>
#include <ZTR_ChunkSAMP.hh>
#include <ZTR_ChunkREGN.hh>
#include <ZTR_DataCompression.hh>
#include <ZTR_DataCompressionRaw.hh>
#include <SRF_util.hh>
#include <SRF_File.hh>
#include <SRF_DataBlock.hh>
#include <SRF_DataBlockHeader.hh>
#include <SRF_ContainerHeader.hh>
#include <SRF_XMLBlock.hh>
#include <SRF_CommandLineArgs.hh>
#include <SRF_SOLiDWriter.hh>

SRF_SOLiDWriter::SRF_SOLiDWriter(
                              const SRF_CommandLineArgs& argsIn )
:
args( argsIn )
{
    outputFile = NULL;
}
    
SRF_SOLiDWriter::~SRF_SOLiDWriter( void )
{
    DELETE( outputFile );
}

bool
SRF_SOLiDWriter::openOutput( void )
{
   SRF_FileOpenType mode = SRF_FileOpenTypeWrite;
    std::string outputFilename;
    args.getArgValue( SRF_ARGS_OUTPUT, &outputFilename );
    outputFile = new SRF_File( outputFilename.c_str(), mode );
    if (!outputFile->open() )
    {
        std::cout << "ERROR: Writer can't open output file" <<  outputFilename << std::endl;
        return FALSE;
    }
    return TRUE;
}

bool
SRF_SOLiDWriter::writeHeaderAndXml( void )
{
    SRF_ContainerHeader containerHeader;

    if ( !containerHeader.setup( SRF_ContainerTypeZTR,
                                 "Sunspot",
                                 "2.0" ) )
    {
        std::cout << "ERROR: Writer can't set up container" <<  std::endl;
        return FALSE;
    }

    if ( !containerHeader.write( outputFile->getFile() ) )
    {
        std::cout << "ERROR: Writer can't set up container" <<  std::endl;
        return FALSE;
    }

    // write XML block if required
    std::string xmlFilename;
    args.getArgValue( SRF_ARGS_XML, &xmlFilename );
    if ( xmlFilename.length() > 0 )
    {
        std::ifstream xmlFile;
        xmlFile.open( xmlFilename.data(),
                      std::fstream::in|std::fstream::binary );
        if ( !xmlFile.good() )
        {
            std::cout << "ERROR: Writer can't read XML file: " <<
                         xmlFilename <<  std::endl;
            return FALSE;
        }

        const static int BUFFER_SIZE = 4096;
        char buffer[BUFFER_SIZE];
        std::string xmlStr = "";
        do
        {
            xmlFile.read(&buffer[0], BUFFER_SIZE);
            xmlStr.append( buffer, xmlFile.gcount() );
        } while (xmlFile.gcount() > 0 );

        xmlFile.close();
            
        SRF_XMLBlock xmlBlock;
        if ( !xmlBlock.setup( xmlStr ) )
        {
               std::cout << "ERROR: Writer can't set up XML block\n";
               return FALSE;
        }

        if ( !xmlBlock.write( outputFile->getFile() ) )
        {
            std::cout << "ERROR: Writer can't write XML block\n";
            return FALSE;
        }
    }

    return TRUE;
}

bool
SRF_SOLiDWriter::createBaseChunk( const std::string& calls,
                                  std::string& blob  )
{
    ZTR_ChunkBase baseChunk;
    if ( !baseChunk.setup( ZTR_ChunkBaseCharSetColourSpace,
                           calls ) )
    {
       std::cout << "ERROR: Writer can't set up Base Chunk\n";
       return FALSE;
    }

    ZTR_DataCompressionRaw rawCompression;
    if ( !baseChunk.readyData( &rawCompression ) )
    {
       std::cout << "ERROR: Writer can't ready Base Chunk\n";
       return FALSE;
    }

    blob += baseChunk.getData();

   return TRUE;
}

bool
SRF_SOLiDWriter::createCnf1Chunk( const std::vector<int>& confs,
                                  std::string& blob  )
{
    ZTR_ChunkCNF1 cnf1Chunk;
    if ( !cnf1Chunk.setup( ZTR_ChunkCNFscalePHRED,
                           confs ) )
    {
        std::cout << "ERROR: Writer can't set up CNF1 Chunk\n";
       return FALSE;
    }

    ZTR_DataCompressionRaw rawCompression;
    if ( !cnf1Chunk.readyData( &rawCompression ) )
    {
        std::cout << "ERROR: Writer can't ready CNF1 Chunk\n";
       return FALSE;
    }
    blob += cnf1Chunk.getData();

   return TRUE;
}

bool
SRF_SOLiDWriter::createRegnChunk( const std::vector<std::string>& names,
                                  const std::vector<ZTR_REGNtypes>& codes,
                                  const std::vector<int>& boundaries,
                                  std::string& blob )
{
    ZTR_ChunkREGN regnChunk;
    if ( !regnChunk.setup( names, codes, boundaries, ZTR_Type16bitInt ) )
    {
        std::cout << "ERROR: Writer can't set up REGN Chunk\n";
       return FALSE;
    }

    ZTR_DataCompressionRaw rawCompression;
    if ( !regnChunk.readyData( &rawCompression ) )
    {
        std::cout << "ERROR: Writer can't ready REGN Chunk\n";
       return FALSE;
    }
    blob += regnChunk.getData();

   return TRUE;
}


bool
SRF_SOLiDWriter::createSampChunks(
                        const std::vector<SRF_IntensityData> intensityData,
                        std::string& blob )
{
    ZTR_DataCompressionRaw rawCompression;

    std::vector<SRF_IntensityData>::const_iterator it;
    it = intensityData.begin();
    while ( it != intensityData.end() )
    {
        ZTR_ChunkSAMP sampChunk;
        if ( !sampChunk.setup( (*it).intensityValues ) )
        {
            std::cout << "ERROR: Writer can't ready SAMP Chunk\n";
            return FALSE;
        }

        std::string sType( (*it).sampName );
        std::string typeStr( "TYPE" );
        sampChunk.getMetadata()->setMetadata( typeStr, (*it).sampName );
        if ( !sampChunk.readyData( &rawCompression ) )
        {
            std::cout << "ERROR: Writer can't ready SAMP Chunk\n";
            return FALSE;
        }

        blob += sampChunk.getData();
        it++;
    }
    return TRUE;
}

bool
SRF_SOLiDWriter::writeFooter( void )
{
    // terminal 8 bytes indicating size of index - no index so set to zero
    char buffer[8];
    memset( buffer, 0, 8 );
    outputFile->getFile().write( buffer, 8 );
 
    return TRUE;
}

bool
SRF_SOLiDWriter::openFileSet( SRF_SOLiDfileSet* fileSet,
                              const char* cfastaCode,
                              const char* qualCode,
                              const char* intensityCode )
{
    std::string cfFile;
    args.getArgValue( cfastaCode, &cfFile );
    if (!fileSet->cfastaFile.open( cfFile,
                          args.retArgValue( SRF_ARGS_TITLE ),
                          args.flagSet( SRF_ARGS_TITLE_OVERRIDE ) ) )
    {
        return FALSE;
    }

    if ( args.flagSet( qualCode ) )
    {
        std::string qualFileName;
        args.getArgValue( qualCode, &qualFileName );
        if (!fileSet->qualFile.open( qualFileName,
                          args.retArgValue( SRF_ARGS_TITLE ),
                          args.flagSet( SRF_ARGS_TITLE_OVERRIDE ) ) )
        {
            return FALSE;
        }
    }

    if ( args.flagSet( intensityCode ) )
    {
        std::string intensityRoot;
        args.getArgValue( intensityCode, &intensityRoot );
        if ( !fileSet->intensityFiles.open( intensityRoot,
                                   args.retArgValue( SRF_ARGS_TITLE ),
                                   args.flagSet( SRF_ARGS_TITLE_OVERRIDE ) ) )
        {
            return FALSE;
        }
    }

   return TRUE;
}

bool
SRF_SOLiDWriter::moreData( SRF_SOLiDfileSet& fileSet,
                                    bool& allFilesEnd )
{
    allFilesEnd = TRUE;
    if ( fileSet.cfastaFile.moreData() &&
         fileSet.qualFile.moreData() &&
         fileSet.intensityFiles.moreData() )
    {
        return TRUE;
    }

    if ( fileSet.cfastaFile.moreData() ||
         ( fileSet.qualFile.isFileOpen() &&
           fileSet.qualFile.moreData() ) ||
         ( fileSet.intensityFiles.areFilesOpen() &&
           fileSet.intensityFiles.moreData() ) )
    {
        allFilesEnd = FALSE; // files have different content
    }

    return FALSE;
}

void
SRF_SOLiDWriter::generatePrefix( const SRF_SOLiDdataSet& dataset,
                                 std::string* prefix ) const
{
    std::string vendorPrefix = "VAB_"; 
    if ( args.flagSet( SRF_ARGS_NO_AB_PREFIX ) )
    {
        vendorPrefix = "";
    }
    *prefix = vendorPrefix +
              args.retArgValue( SRF_ARGS_TITLE ) +
              dataset.panelId;
}
