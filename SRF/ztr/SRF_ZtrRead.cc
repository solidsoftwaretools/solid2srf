//Copyright 2008 Asim Siddiqui
//
// License TBD
//
#include <SRF_ZtrRead.hh>
#include <SRF_ContainerHeader.hh>
#include <SRF_DataBlockHeader.hh>
#include <SRF_DataBlock.hh>


SRF_ZtrRead::SRF_ZtrRead( void )
{
    pairedEndRead = FALSE;
}

SRF_ZtrRead::SRF_ZtrRead( SRF_ContainerHeader* containerHeaderPtr,
                    SRF_DataBlockHeader* dataBlockHeaderPtr,
                    ABANDON SRF_DataBlock* dataBlockPtr )
:
SRF_Read( containerHeaderPtr, dataBlockHeaderPtr, dataBlockPtr )
{
    pairedEndRead = FALSE;
}

SRF_ZtrRead::~SRF_ZtrRead( void )
{
  // null
}

bool
SRF_ZtrRead::extract( bool splitPair, matchParams )
{
    if ( !chunkList.extract( ztrBlob ) )
    {
        SRF_ReportError( "ERROR: Unable to read blob\n" );
        return FALSE;
    }

    const ZTR_ChunkREGN* regnChunk = NULL;
    const std::vector<ZTR_Chunk*>& chunks = chunkList->getChunks();
    std::vector<ZTR_Chunk*>::const_iterator chunkIt = chunks.begin();
    while ( chunkIt != chunks.end() )
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
            chunkIt = chunks.end();
        }
        else
        {
            chunkIt++:
        }
    }

    chunkIt = chunks.begin();
    while ( chunkIt != chunks.end() )
    {
        const ZTR_Chunk* chunk = *chunkIt;

        if ( chunk->type() == ZTR_ChunkTypeBASE )
        {
            if ( regnChunk == NULL )
            {
                bases = chunk->getUncompressedDataStr();
            }
            else
            {
                // TODO
            }
        }
        else if ( chunk->type() == ZTR_ChunkTypeCNF1 )
        {
}

void
SRF_ZtrRead::matchChunk( const ZTR_Chunk* chunk, matchParams )
{


std::string&
SRF_ZtrRead::getBases( void ) const
{
    return bases;
}

const std::vector<int>&
SRF_ZtrRead::getQualities( void ) const
{
    return qualities;
}

const std::vector<float>&
SRF_ZtrRead::getProcessedIntenstities( void ) const
{
    return processedIntensities;
}

std::string&
SRF_ZtrRead::getBases2of2( void ) const
{
    return bases2of2;
}
const std::vector<int>&
SRF_ZtrRead::getQualities2of2( void ) const
{
    return qualities2of2;
}
const std::vector<float>&
SRF_ZtrRead::getProcessedIntenstities2of2( void ) const
{
    return processedIntensities2of2;
}

bool isPairedEndRead( void ) const
{
    return pairedEndRead;
}
