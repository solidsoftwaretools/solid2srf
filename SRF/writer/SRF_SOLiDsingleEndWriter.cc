//
#include <iostream>
#include <map>
#include <string>
#include "ZTR_util.hh"
#include "SRF_util.hh"
#include "SRF_DataBlock.hh"
#include "SRF_DataBlockHeader.hh"
#include "SRF_SOLiDsingleEndWriter.hh"

SRF_SOLiDsingleEndWriter::SRF_SOLiDsingleEndWriter(
                              const SRF_CommandLineArgs& argsIn )
: SRF_SOLiDWriter( argsIn )
{
    // no op
}
    
SRF_SOLiDsingleEndWriter::~SRF_SOLiDsingleEndWriter( void )
{
    // no-op
}


bool
SRF_SOLiDsingleEndWriter::open( void )
{
    if ( !openOutput() )
    {
        return FALSE;
    }

    if ( !openFileSet( &fileSet,
                       SRF_ARGS_CFASTA,
                       SRF_ARGS_QUAL,
                       SRF_ARGS_INTENSITY ) )
    {
            return FALSE;
    }

    return TRUE;
}

bool
SRF_SOLiDsingleEndWriter::moreData( bool& allFilesEnd )
{
    return( SRF_SOLiDWriter::moreData( fileSet, allFilesEnd) );
}

bool
SRF_SOLiDsingleEndWriter::writeNextBlock( void )
{
    prevDataSet = currDataSet;
    if ( !fileSet.cfastaFile.readNextBlock( &currDataSet ) ||
         !fileSet.qualFile.readNextBlock( currDataSet.partialReadId,
                                          &currDataSet ) ||
         !fileSet.intensityFiles.readNextBlock( currDataSet.partialReadId, 
                                          &currDataSet ) )
    {
         std::cout << "ERROR: unexpected file format near read id "
                   << prevDataSet.partialReadId << std::endl;
         return FALSE;
    }


    if ( currDataSet.panelId != prevDataSet.panelId )
    {
        if ( !writeDBH() )
        {
            std::cout << "ERROR: unable to write new DBH at read id: "  <<
                currDataSet.partialReadId << std::endl;
            return FALSE;
        }
    }

   if ( !writeDB() )
   {
        std::cout << "ERROR: unable to write DB at read id: "  <<
            currDataSet.partialReadId << std::endl;
        return FALSE;
   }

   return TRUE;
}

void
SRF_SOLiDsingleEndWriter::modifyDataByREGNBlockContent( void )
{
    // delete insert data depending on whats in the REGN block
    if ( args.flagSet( SRF_ARGS_NO_PRIMER_BASE_IN_REGN ) )
    {
        if ( fileSet.qualFile.isFileOpen() )
        {
            currDataSet.confValues.insert( currDataSet.confValues.begin(),
                                           255 );
        }

        if ( fileSet.intensityFiles.areFilesOpen() )
        {
            currDataSet.intensityData[0].intensityValues.insert(
                currDataSet.intensityData[0].intensityValues.begin(),
                -1 );
            currDataSet.intensityData[1].intensityValues.insert(
                currDataSet.intensityData[1].intensityValues.begin(),
                -1 );
            currDataSet.intensityData[2].intensityValues.insert(
                currDataSet.intensityData[2].intensityValues.begin(),
                -1 );
            currDataSet.intensityData[3].intensityValues.insert(
                currDataSet.intensityData[3].intensityValues.begin(),
                -1 );
        }
    }
    else
    {
        currDataSet.calls.erase( 0, 1 );
    }
}

bool
SRF_SOLiDsingleEndWriter::createRegnChunk( std::string& blob )
{
    if ( args.flagSet( SRF_ARGS_NO_PRIMER_BASE_IN_REGN ) )
    {
        return TRUE;
    }

    std::vector<std::string> names;
    std::vector<ZTR_REGNtypes> codes;
    std::vector<int> boundaries;

    std::string lastBaseOfPrimer( currDataSet.calls, 0, 1 );
    names.push_back( lastBaseOfPrimer );
    names.push_back( "SEQ" );

    codes.push_back( ZTR_REGNtypesExplicit );
    codes.push_back( ZTR_REGNtypesNormal );

    boundaries.push_back( 0 );

    return ( SRF_SOLiDWriter::createRegnChunk( names, codes, boundaries, blob));
}

bool
SRF_SOLiDsingleEndWriter::writeDBH( void )
{
    std::string uniqueIdPrefix;
    generatePrefix( currDataSet, &uniqueIdPrefix );

    std::string blob =  ZTR_Header();
    if ( !createRegnChunk( blob ) )
    {
        std::cout << "ERROR: Writer can't set up DBH at Read Id:" <<
                currDataSet.partialReadId << std::endl;
        return FALSE;
    }

    SRF_DataBlockHeader dataBlockHeader;
    if ( !dataBlockHeader.setup( uniqueIdPrefix, blob ) )
    {
        std::cout << "ERROR: Writer can't set up DBH at Read Id:" <<
            currDataSet.partialReadId << std::endl;
        return FALSE;
    }

    if ( !dataBlockHeader.write( outputFile->getFile() ) )
    {
        std::cout << "ERROR: Writer can't write DBH at Read Id:" <<
            currDataSet.partialReadId << std::endl;
        return FALSE;
    }

    return TRUE;
}

bool
SRF_SOLiDsingleEndWriter::writeDB( void )
{
    modifyDataByREGNBlockContent();
    std::string blob = "";
    if ( !createBaseChunk( currDataSet.calls, blob ) )
    {
        std::cout << "ERROR: Writer can't set up Base Chunk at Read Id:" <<
                        currDataSet.panelId <<
                        currDataSet.readIdSuffix <<  std::endl;
        return FALSE;
    }

    if ( fileSet.qualFile.isFileOpen() &&
         !createCnf1Chunk( currDataSet.confValues, blob ))
    {
        std::cout << "ERROR: Writer can't set up CNF1 Chunk at Read Id:" <<
                        currDataSet.panelId <<
                        currDataSet.readIdSuffix <<  std::endl;
        return FALSE;
    }

    if ( fileSet.intensityFiles.areFilesOpen() &&
         !createSampChunks( currDataSet.intensityData, blob ) )
    {
        std::cout << "ERROR: Writer can't set up SAMP Chunks at Read Id:" <<
                        currDataSet.panelId <<
                        currDataSet.readIdSuffix <<  std::endl;
        return FALSE;
    }

    SRF_DataBlock dataBlock;
    if ( !dataBlock.setup( currDataSet.readIdSuffix, blob ) )
    {
        std::cout << "ERROR: Writer can't set up DB at Read Id:" <<
                        currDataSet.panelId <<
                        currDataSet.readIdSuffix <<  std::endl;
        return FALSE;
    }

    if ( !dataBlock.write( outputFile->getFile() ) )
    {
        std::cout << "ERROR: Writer can't write DB at Read Id:" <<
                        currDataSet.panelId <<
                        currDataSet.readIdSuffix <<  std::endl;
        return FALSE;
    }

    return TRUE;
}
