//Copyright 2008 Asim Siddiqui
//
// License TBD
//
#include <ZTR_Chunk.hh>
#include <ZTR_ChunkREGN.hh>
#include <ZTR_RetrieveChunk.hh>
#include <ZTR_RetrieveChunk.hh>
#include <sstream>
#include <cstdio>

#ifdef HAVE_CONFIG_H
#include <srf_config.h>
#endif

ZTR_RetrieveChunk::ZTR_RetrieveChunk( void )
:
matchedChunk( NULL ),
regnChunk( NULL )
{
    pairedEndRead = FALSE;
}

ZTR_RetrieveChunk::~ZTR_RetrieveChunk( void )
{
  // null
}

bool
ZTR_RetrieveChunk::extract( const ZTR_ChunkList& chunkList,
                            const ZTR_ChunkMatchParams& matchParams,
                            bool splitPair )
{
    regnChunk = NULL;
    const std::vector<ZTR_Chunk*>& chunks = chunkList.getChunks();
    std::vector<ZTR_Chunk*>::const_iterator chunkIt = chunks.begin();
    bool chunkFound = FALSE;
    while ( chunkIt != chunks.end() && !chunkFound )
    {
        const ZTR_Chunk* chunk = *chunkIt;

        if ( chunk->type() == ZTR_ChunkTypeREGN )
        {
            regnChunk = (const ZTR_ChunkREGN*) chunk;
            const std::vector<ZTR_REGNtypes>& codes = regnChunk->getCodes();

            std::vector<ZTR_REGNtypes>::const_iterator it =
                codes.begin();
            while ( it != codes.end() )
            {
                if ( *it == ZTR_REGNtypesPaired )
                {
                    pairedEndRead = TRUE;
                    it = codes.end();
                }
                else
                {
                    it++;
                }
            }
        }
        else
        {
            chunkFound =  matchChunk( chunk, matchParams );
        }
        chunkIt++;
    }

    if ( !chunkFound )
    {
        ZTR_ReportError( "requested chunk not found" );
        return FALSE;
    }

    if ( !extractData( matchParams, splitPair ) )
    {
        ZTR_ReportError( "problem in chunk format" );
        return FALSE;
    }


    return TRUE;
}

bool
ZTR_RetrieveChunk::matchChunk( const ZTR_Chunk* chunk,
                               const ZTR_ChunkMatchParams& matchParams )
{
    if ( chunk->type() != matchParams.type )
    {
        return FALSE;
    }

    if ( !(matchParams.metadata.filter( *chunk->getMetadataConst())) )
    {
        return FALSE;
    }

    matchedChunk = chunk;
    return TRUE;
}

bool
ZTR_RetrieveChunk::extractData( const ZTR_ChunkMatchParams& matchParams,
                                bool splitPair )
{
    const ZTR_Data& matchedChunkData = matchedChunk->getAssignedData();
    data = matchedChunkData;

    if ( matchParams.type == ZTR_ChunkTypeBase )
    {
        // review 'Ns' and reinsert
        const std::vector<ZTR_REGNtypes>& codes = regnChunk->getCodes();
        const std::vector<std::string>& names = regnChunk->getNames();

        std::vector<ZTR_REGNtypes>::const_iterator itC = codes.end()-1;
        std::vector<std::string>::const_iterator itN = names.end()-1;
        int ii = codes.size() - 1;
        while ( ii >= 0 )
        {
            if ( *itC == ZTR_REGNtypesExplicit )
            {
                data.dataString.insert(
                         (regnChunk->getUncompressedDataInt())[ii],
                         *itN );
            }

            itC--;
            itN--;
            ii--;
        }
    } else if (matchParams.type == ZTR_ChunkTypeCNF1 )
    {
        // Remap QVs of 255 to -1
        // Allows for round trip of QV -1 data
        std::vector<int>::iterator it = data.ints.begin();
        while( it != data.ints.end() )
        {
            if( *it >= 255 ) {
                *it = -1;
            }
            ++it;
        }
    }
    
    if ( splitPair && pairedEndRead )
    {
        //split after first P
        const std::vector<ZTR_REGNtypes>& codes = regnChunk->getCodes();
        const std::vector<std::string>& names = regnChunk->getNames();

        std::vector<ZTR_REGNtypes>::const_iterator it = codes.begin();
        int ii = 0;
        int numCharsInExplicit = 0;
        while ( *it != ZTR_REGNtypesPaired )
        {
            if ( *it == ZTR_REGNtypesExplicit )
            {
                numCharsInExplicit += names[ii].size();
            }

            it++;
            ii++;
        }

        if ( it == codes.end() )
        {
            ZTR_ReportError( "requested chunk not found" );
            return FALSE;
        }

        pairedEndName1 = names[ii];
        int jj = ii + 1;
        while ( codes[jj] != ZTR_REGNtypesPaired )
        {
            jj++;
        }
        pairedEndName2 = names[jj];
//        ii++; // this increments to 1 past the end of the first paired end read

        int split = (regnChunk->getUncompressedDataInt())[ii];

        data2of2 = data;
        if ( data.ints.size() > 0 )
        {
            data.ints.erase( data.ints.begin()+split,
                             data.ints.end() );
            data2of2.ints.erase( data2of2.ints.begin(),
                                 data2of2.ints.begin()+split );
        }
        else if ( data.floats.size() > 0 )
        {
            data.floats.erase( data.floats.begin()+split,
                               data.floats.end() );
            data2of2.floats.erase( data2of2.floats.begin(),
                                   data2of2.floats.begin()+split );
        }
        else if ( data.dataString.size() > 0 )
        {
            split += numCharsInExplicit;
            data.dataString.erase( data.dataString.begin()+split,
                                   data.dataString.end() );
            data2of2.dataString.erase( data2of2.dataString.begin(),
                                       data2of2.dataString.begin()+split );
        }
    }

    return TRUE;
}

const ZTR_Data&
ZTR_RetrieveChunk::getData( void ) const
{
    return data;
}

const ZTR_Data&
ZTR_RetrieveChunk::getData2of2( void ) const
{
    return data2of2;
}

bool
ZTR_RetrieveChunk::isPairedEndRead( void ) const
{
    return pairedEndRead;
}

std::string
ZTR_RetrieveChunk::getDataForOutput( void ) const
{
    return getDataForOutputGen( data );
}

std::string
ZTR_RetrieveChunk::getData2of2ForOutput( void ) const
{
    return getDataForOutputGen( data2of2 );
}


std::string
ZTR_RetrieveChunk::getDataForOutputGen( const ZTR_Data& dataIn ) const
{
    std::ostringstream oss( "" );
    if ( dataIn.dataString.size() > 0 )
    {
        oss << dataIn.dataString;
    }
    else if ( dataIn.ints.size() > 0 )
    {
        std::vector<int>::const_iterator it = dataIn.ints.begin();        
        while ( it != dataIn.ints.end() )
        {
            oss << (*it) << " ";
            it++;
        }
    }
    else if ( dataIn.floats.size() > 0 )
    {
        std::vector<float>::const_iterator it = dataIn.floats.begin();
        char buffer[100];
        while ( it != dataIn.floats.end() )
        {
            snprintf( buffer, 100, "%.6g", (*it) );
#ifdef BROKEN_PRINTF
            if ( ((*it) > 0 && strncmp( buffer, "1e-04", 5 ) == 0 ) ||
                 ((*it) < 0 && strncmp( buffer, "-1e-04", 6 ) == 0 ) )
            {
                snprintf( buffer, 100, "%.4f", (*it) );
            }
#endif // BROKEN_PRINTF
            oss << buffer << " ";
            it++;
        }
    }

    oss << std::endl;

    return oss.str();
}

const std::string&
ZTR_RetrieveChunk::getPairedEndName1( void ) const
{
    return pairedEndName1;
}

const std::string&
ZTR_RetrieveChunk::getPairedEndName2( void ) const
{
    return pairedEndName2;
}


