//
#include <map>
#include <string>
#include <sstream>
#include <ZTR_util.hh>
#include <SRF_util.hh>
#include <SRF_DataBlock.hh>
#include <SRF_DataBlockHeader.hh>
#include <SRF_SOLiDpairedEndWriter.hh>

SRF_SOLiDpairedEndWriter::SRF_SOLiDpairedEndWriter(
                              const SRF_CommandLineArgs& argsIn )
: SRF_SOLiDWriter( argsIn )
{
    // no op
}
    
SRF_SOLiDpairedEndWriter::~SRF_SOLiDpairedEndWriter( void )
{
    // no-op
}


bool
SRF_SOLiDpairedEndWriter::open( void )
{
    if ( !openOutput() )
    {
        return FALSE;
    }

    if ( !openFileSet( &fileSet1,
                       SRF_ARGS_CFASTA,
                       SRF_ARGS_QUAL,
                       SRF_ARGS_INTENSITY ) )
    {
            return FALSE;
    }

    if ( !openFileSet( &fileSet2,
                       SRF_ARGS_CFASTA2,
                       SRF_ARGS_QUAL2,
                       SRF_ARGS_INTENSITY2 ) )
    {
            return FALSE;
    }

    // insert dummy rows into dataSetsFile
    SRF_SOLiDdataSet dummy;
    dummy.panelId = "";
    dataSetsFile1.push_back( dummy );
    dataSetsFile2.push_back( dummy );

    return TRUE;
}

bool
SRF_SOLiDpairedEndWriter::moreData( bool& allFilesEnd )
{
    bool allFileSet1Ends, allFileSet2Ends;

    fileSet1HasMoreData = SRF_SOLiDWriter::moreData( fileSet1,
                                                      allFileSet1Ends);
    fileSet2HasMoreData = SRF_SOLiDWriter::moreData( fileSet2,
                                                      allFileSet2Ends);

    if ( allFileSet1Ends && allFileSet2Ends )
    {
        allFilesEnd = TRUE;
    }
    else
    {
        allFilesEnd = FALSE;
    }

    if ( fileSet1HasMoreData || fileSet2HasMoreData ||
         dataSetsFile1.size() > 1 || dataSetsFile2.size() > 1 )
    {
        return TRUE;
    }

    return FALSE;
}

bool
SRF_SOLiDpairedEndWriter::writeNextBlock( void )
{
    SRF_SOLiDdataSet iDataSetFile1, iDataSetFile2;
    if ( fileSet1HasMoreData && dataSetsFile1.size() < 2 )
    {
        if ( !fileSet1.cfastaFile.readNextBlock( &iDataSetFile1 ) ||
             !fileSet1.qualFile.readNextBlock( iDataSetFile1.partialReadId,
                                              &iDataSetFile1 ) ||
             !fileSet1.intensityFiles.readNextBlock(
                                              iDataSetFile1.partialReadId,
                                              &iDataSetFile1 ) )
        {
            // TODO get readId
             std::cout << "ERROR: unexpected file format near read id\n";
             return FALSE;
        }
        dataSetsFile1.push_back( iDataSetFile1 );
    }

    if ( fileSet2HasMoreData && dataSetsFile2.size() < 2 )
    {
        if ( !fileSet2.cfastaFile.readNextBlock( &iDataSetFile2 ) ||
             !fileSet2.qualFile.readNextBlock( iDataSetFile2.partialReadId,
                                              &iDataSetFile2 ) ||
             !fileSet2.intensityFiles.readNextBlock(
                                              iDataSetFile2.partialReadId,
                                              &iDataSetFile2 ) )
        {
            // TODO get readId
             std::cout << "ERROR: unexpected file format near read id\n";
             return FALSE;
        }
        dataSetsFile2.push_back( iDataSetFile2 );
    }

    if ( !identifyNextPairToWrite() )
    {
        // TODO get readId
        std::cout << "ERROR: unexpected file format near read id\n";
        return FALSE;
    }

// prev pair is now in index[0], new pair is in index[1], blank filled
// as required. For the first line, a blank dataset was inserted in index[0].
// the blank dataset is correctly filled with appropriate values

    if ( dataSetsFile1[1].panelId != // doesn't matter whether we check
         dataSetsFile1[0].panelId )  // dataset1 or 2
    {
        if ( !writeDBH() )
        {
            std::cout << "ERROR: unable to write new DBH at read id: "  <<
                dataSetsFile1[1].partialReadId << std::endl;
            return FALSE;
        }
    }

   if ( !writeDB() )
   {
        std::cout << "ERROR: unable to write DB at read id: "  <<
                dataSetsFile1[1].partialReadId << std::endl;
        return FALSE;
   }

   // take the first row off each list, unless we have reached the end
   // of the file and didn't get any more data
   if ( dataSetsFile1.size() > 1 )
   {
       dataSetsFile1.erase( dataSetsFile1.begin() );
   }
   if ( dataSetsFile2.size() > 1 )
   {
       dataSetsFile2.erase( dataSetsFile2.begin() );
   }

   return TRUE;
}

bool
SRF_SOLiDpairedEndWriter::identifyNextPairToWrite( void )
{
    bool dummyOn1 = FALSE;
    if ( fileSet1HasMoreData && fileSet2HasMoreData )
    {
        std::string trPartialReadId1 = dataSetsFile1[1].partialReadId;
        size_t found = trPartialReadId1.find( '_' );
        while ( found != std::string::npos )
        {
            trPartialReadId1[found] = ' ';
            found = trPartialReadId1.find( '_' );
        }

        std::string trPartialReadId2 = dataSetsFile2[1].partialReadId;
        found = trPartialReadId2.find( '_' );
        while ( found != std::string::npos )
        {
            trPartialReadId2[found] = ' ';
            found = trPartialReadId2.find( '_' );
        }

        std::istringstream partialReadId1( trPartialReadId1 );
        std::istringstream partialReadId2( trPartialReadId2 );

        int panelId1, panelId2, x1, x2, y1, y2;
        std::string suffix1, suffix2;
        partialReadId1 >> panelId1 >> x1 >> y1 >> suffix1;
        partialReadId2 >> panelId2 >> x2 >> y2 >> suffix2;

        if ( panelId1 == panelId2 )
        {
            if ( x1 == x2 )
            {
                if ( y1 == y2 )
                {
                    return TRUE;
                }
                else if ( y1 < y2 )
                {
                    dummyOn1 = FALSE;
                }
                else
                {
                    dummyOn1 = TRUE;
                }
            }
            else if ( x1 < x2 )
            {
                dummyOn1 = FALSE;
            }
            else
            {
                dummyOn1 = TRUE;
            } 
        }
        else if ( panelId1 < panelId2 )
        {
            dummyOn1 = FALSE;
        }
        else
        {
            dummyOn1 = TRUE;
        } 
    }
    else if ( fileSet1HasMoreData )
    {
        dummyOn1 = FALSE;
    }
    else
    {
        dummyOn1 = TRUE;
    }
 
    if ( dummyOn1 )
    {
        insertDummy( dataSetsFile1, dataSetsFile2 );
    }
    else
    {
        insertDummy( dataSetsFile2, dataSetsFile1 );
    }

    return TRUE;
}

void
SRF_SOLiDpairedEndWriter::insertDummy(
                             std::vector<SRF_SOLiDdataSet>& dummyInsertSet,
                       const std::vector<SRF_SOLiDdataSet>& goodSet )
{
    // have to insert a dummy SRF_SOLiDdataSet object into one set
    // to account for a missing read
    // the dummy mostly takes its "valid params" e.g. panel id from the
    // good set, but the length of the dummy fields are taken from the
    // member of the dummy set
    // default colour = '.' default conf = 0 default intensity = -1

    // the size of the dummy fields is taken from the second object if there
    // is one or the first one. This handles edge effects at both the start
    // and end of the file

    int dummyInsertSetCopyRow = 1;
    if ( dummyInsertSet.size() > 1 )
    {
        dummyInsertSetCopyRow = 1;
    }
    else if ( dummyInsertSet.size() == 1 )
    {
        dummyInsertSetCopyRow = 0;
    }
    else
    {
        std::cout << "ERROR: s/w error" << std::endl;
        exit(-1);
    }

    SRF_SOLiDdataSet dummy;
    dummy.partialReadId = goodSet[1].partialReadId;
    dummy.panelId = goodSet[1].panelId;
    dummy.readIdSuffix = goodSet[1].readIdSuffix;

    dummy.calls.insert( dummy.calls.begin(),
                        dummyInsertSet[dummyInsertSetCopyRow].calls.size(),
                        '.' );

    if ( dummyInsertSet[dummyInsertSetCopyRow].confValues.size() > 0 )
    {
        dummy.confValues.insert( dummy.confValues.begin(),
                                 dummyInsertSet[dummyInsertSetCopyRow].confValues.size(), 
                                 0 );
    }

    if ( dummyInsertSet[dummyInsertSetCopyRow].intensityData.size() > 0 )
    {
        SRF_IntensityData dummyIntensityData;
        dummyIntensityData.intensityValues.insert(
                    dummyIntensityData.intensityValues.begin(),
                    dummyInsertSet[dummyInsertSetCopyRow].intensityData[0].intensityValues.size(),
                    -1.0 );

        dummyIntensityData.sampName =
                               dummyInsertSet[dummyInsertSetCopyRow].intensityData[0].sampName;
        dummy.intensityData.push_back( dummyIntensityData );

        dummyIntensityData.sampName =
                               dummyInsertSet[dummyInsertSetCopyRow].intensityData[1].sampName;
        dummy.intensityData.push_back( dummyIntensityData );

        dummyIntensityData.sampName =
                               dummyInsertSet[dummyInsertSetCopyRow].intensityData[2].sampName;
        dummy.intensityData.push_back( dummyIntensityData );

        dummyIntensityData.sampName =
                               dummyInsertSet[dummyInsertSetCopyRow].intensityData[3].sampName;
        dummy.intensityData.push_back( dummyIntensityData );
    }

// dummy is inserted at position 1
    dummyInsertSet.insert( dummyInsertSet.begin() + 1, dummy );
}

bool
SRF_SOLiDpairedEndWriter::createRegnChunk( std::string& blob )
{
    std::vector<std::string> names;
    std::vector<ZTR_REGNtypes> codes;
    std::vector<int> boundaries;

    std::string name1 = dataSetsFile1[1].readIdSuffix.substr(
                        dataSetsFile1[1].readIdSuffix.find_last_of( '_' ) + 1 );
    std::string name2 = dataSetsFile2[1].readIdSuffix.substr(
                        dataSetsFile2[1].readIdSuffix.find_last_of( '_' ) + 1 );

    if ( args.flagSet( SRF_ARGS_NO_PRIMER_BASE_IN_REGN ) )
    {
        names.push_back( name1 );
        names.push_back( name2 );

        codes.push_back( ZTR_REGNtypesPaired );
        codes.push_back( ZTR_REGNtypesPaired );

        boundaries.push_back( dataSetsFile1[1].calls.length() );
    }
    else
    {
        std::string lastBaseOfPrimer1( dataSetsFile1[1].calls, 0, 1 );
        std::string lastBaseOfPrimer2( dataSetsFile2[1].calls, 0, 1 );

        names.push_back( lastBaseOfPrimer1 );
        names.push_back( name1 );
        names.push_back( lastBaseOfPrimer2 );
        names.push_back( name2 );

        codes.push_back( ZTR_REGNtypesExplicit );
        codes.push_back( ZTR_REGNtypesPaired );
        codes.push_back( ZTR_REGNtypesExplicit );
        codes.push_back( ZTR_REGNtypesPaired );

        boundaries.push_back( 0 );
        boundaries.push_back( dataSetsFile1[1].calls.length() );
        boundaries.push_back( dataSetsFile1[1].calls.length() ); // same
                                                                 // boundary
    }

    return ( SRF_SOLiDWriter::createRegnChunk( names, codes, boundaries, blob));
}
    
bool
SRF_SOLiDpairedEndWriter::writeDBH( void )
{
    std::string uniqueIdPrefix;
    generatePrefix( dataSetsFile1[1], &uniqueIdPrefix);

    std::string blob =  ZTR_Header();

    if ( !createRegnChunk( blob ) )
    {
        std::cout << "ERROR: Writer can't set up DBH at Read Id:" <<
                dataSetsFile1[1].partialReadId << std::endl;
        return FALSE;
    }


    SRF_DataBlockHeader dataBlockHeader;
    if ( !dataBlockHeader.setup( uniqueIdPrefix, blob ) )
    {
        std::cout << "ERROR: Writer can't set up DBH at Read Id:" <<
                dataSetsFile1[1].partialReadId << std::endl;
        return FALSE;
    }

    if ( !dataBlockHeader.write( outputFile->getFile() ) )
    {
        std::cout << "ERROR: Writer can't write DBH at Read Id:" <<
                dataSetsFile1[1].partialReadId << std::endl;
        return FALSE;
    }

    return TRUE;
}

void
SRF_SOLiDpairedEndWriter::modifyDataByREGNBlockContent( void )
{
    // delete insert data depending on whats in the REGN block
    if ( args.flagSet( SRF_ARGS_NO_PRIMER_BASE_IN_REGN ) )
    {
        if ( fileSet1.qualFile.isFileOpen() )
        {
            dataSetsFile1[1].confValues.insert(
                                           dataSetsFile1[1].confValues.begin(),
                                           255 );
            dataSetsFile2[1].confValues.insert(
                                           dataSetsFile2[1].confValues.begin(),
                                           255 );
        }

        if ( fileSet1.intensityFiles.areFilesOpen() )
        {
            dataSetsFile1[1].intensityData[0].intensityValues.insert(
                dataSetsFile1[1].intensityData[0].intensityValues.begin(),
                -1 );
            dataSetsFile1[1].intensityData[1].intensityValues.insert(
                dataSetsFile1[1].intensityData[1].intensityValues.begin(),
                -1 );
            dataSetsFile1[1].intensityData[2].intensityValues.insert(
                dataSetsFile1[1].intensityData[2].intensityValues.begin(),
                -1 );
            dataSetsFile1[1].intensityData[3].intensityValues.insert(
                dataSetsFile1[1].intensityData[3].intensityValues.begin(),
                -1 );

            dataSetsFile2[1].intensityData[0].intensityValues.insert(
                dataSetsFile2[1].intensityData[0].intensityValues.begin(),
                -1 );
            dataSetsFile2[1].intensityData[1].intensityValues.insert(
                dataSetsFile2[1].intensityData[1].intensityValues.begin(),
                -1 );
            dataSetsFile2[1].intensityData[2].intensityValues.insert(
                dataSetsFile2[1].intensityData[2].intensityValues.begin(),
                -1 );
            dataSetsFile2[1].intensityData[3].intensityValues.insert(
                dataSetsFile2[1].intensityData[3].intensityValues.begin(),
                -1 );
        }
    }
    else
    {
        dataSetsFile1[1].calls.erase( 0, 1 );
        dataSetsFile2[1].calls.erase( 0, 1 );
    }
}

bool
SRF_SOLiDpairedEndWriter::writeDB( void )
{
    modifyDataByREGNBlockContent();
    std::string blob = "";
    if ( !createBaseChunk( dataSetsFile1[1].calls + dataSetsFile2[1].calls,
                           blob ) )
    {
        std::cout << "ERROR: Writer can't set up Base Chunk at Read Id:" <<
                        dataSetsFile1[1].panelId <<
                        dataSetsFile1[1].readIdSuffix <<  std::endl;
        return FALSE;
    }

    std::vector<int> confValuesJoined = dataSetsFile1[1].confValues;
    confValuesJoined.insert( confValuesJoined.end(),
                             dataSetsFile2[1].confValues.begin(),
                             dataSetsFile2[1].confValues.end() );
    if ( confValuesJoined.size() > 0 && !createCnf1Chunk( confValuesJoined,
                                                          blob ))
    {
        std::cout << "ERROR: Writer can't set up CNF1 Chunk at Read Id:" <<
                        dataSetsFile1[1].panelId <<
                        dataSetsFile1[1].readIdSuffix <<  std::endl;
        return FALSE;
    }

    if ( dataSetsFile1[1].intensityData.size() > 0 )
    {
        std::vector<SRF_IntensityData> intensityDataJoined =
                                         dataSetsFile1[1].intensityData;
        // safe to assume that the intensity data is ordered the same
        // join intensity data
        intensityDataJoined[0].intensityValues.insert(
                 intensityDataJoined[0].intensityValues.end(),
                 dataSetsFile2[1].intensityData[0].intensityValues.begin(),
                 dataSetsFile2[1].intensityData[0].intensityValues.end() );
        intensityDataJoined[1].intensityValues.insert(
                 intensityDataJoined[1].intensityValues.end(),
                 dataSetsFile2[1].intensityData[1].intensityValues.begin(),
                 dataSetsFile2[1].intensityData[1].intensityValues.end() );
        intensityDataJoined[2].intensityValues.insert(
                 intensityDataJoined[2].intensityValues.end(),
                 dataSetsFile2[1].intensityData[2].intensityValues.begin(),
                 dataSetsFile2[1].intensityData[2].intensityValues.end() );
        intensityDataJoined[3].intensityValues.insert(
                 intensityDataJoined[3].intensityValues.end(),
                 dataSetsFile2[1].intensityData[3].intensityValues.begin(),
                 dataSetsFile2[1].intensityData[3].intensityValues.end() );
        // 

        if ( intensityDataJoined[0].intensityValues.size() > 0 &&
             !createSampChunks( intensityDataJoined, blob ) )
        {
            std::cout << "ERROR: Writer can't set up SAMP Chunks at Read Id:" <<
                            dataSetsFile1[1].panelId <<
                            dataSetsFile1[1].readIdSuffix <<  std::endl;
            return FALSE;
        }
    }

    SRF_DataBlock dataBlock;
    // remove the characters following the last underscore
    int find = dataSetsFile1[1].readIdSuffix.find_last_of('_');
    std::string readId = dataSetsFile1[1].readIdSuffix.substr( 0, find - 1 );
    if ( !dataBlock.setup( readId, blob ) )
    {
        std::cout << "ERROR: Writer can't set up DB at Read Id:" <<
                        dataSetsFile1[1].panelId <<
                        dataSetsFile1[1].readIdSuffix <<  std::endl;
        return FALSE;
    }

    if ( !dataBlock.write( outputFile->getFile() ) )
    {
        std::cout << "ERROR: Writer can't write DB at Read Id:" <<
                        dataSetsFile1[1].panelId <<
                        dataSetsFile1[1].readIdSuffix <<  std::endl;
        return FALSE;
    }

    return TRUE;
}
