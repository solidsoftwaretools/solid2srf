//
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <string>

#include "ZTR_util.hh"
#include "ZTR_Chunk.hh"
#include "ZTR_ChunkBase.hh"
#include "ZTR_ChunkCNF1.hh"
#include "ZTR_ChunkSAMP.hh"
#include "ZTR_ChunkREGN.hh"
#include "ZTR_DataCompression.hh"

#define CHUNK_BASE "BASE"
#define CHUNK_CNF1 "CNF1"
#define CHUNK_SAMP "SAMP"
#define CHUNK_REGN "REGN"

ZTR_Chunk::ZTR_Chunk( void )
{
    // no-op
}
    
ZTR_Chunk::~ZTR_Chunk( void )
{
    // no-op
}

ZTR_ChunkType
ZTR_GetNewChunk( std::istream& dataIn, ZTR_Chunk** newChunk )
{
    *newChunk = NULL;
    std::string chunkTypeIn;
    dataIn >> std::setw( 4 ) >> chunkTypeIn;

    if ( chunkTypeIn.compare( CHUNK_BASE ) == 0 )
    {
        *newChunk = new ZTR_ChunkBase;
    }
    else if ( chunkTypeIn.compare( CHUNK_CNF1 ) == 0 )
    {
        *newChunk = new ZTR_ChunkCNF1;
    }
    else if ( chunkTypeIn.compare( CHUNK_SAMP ) == 0 )
    {
        *newChunk = new ZTR_ChunkSAMP;
    }
    else if ( chunkTypeIn.compare( CHUNK_REGN ) == 0 )
    {
        *newChunk = new ZTR_ChunkREGN;
    }
    else
    {
        ZTR_ReportError( "unknown chunk type" );
        return ZTR_ChunkTypeNone;
    }

    if ( !(*newChunk)->initialize( dataIn ) )
    {
        ZTR_ReportError( "chunk failed to initialize" );
        DELETE( (*newChunk) );
        return ZTR_ChunkTypeNone;
    }
    return (*newChunk)->type();
}

bool
ZTR_Chunk::initialize( std::istream& inputData )
{
    int metadataLength = 0;
    inputData.read( reinterpret_cast<char *>(&metadataLength), 4 );
    metadataLength = ZTR_ByteSwapAsRequired_4( metadataLength );

    char* metadataBuffer = new char[metadataLength];
    inputData.read( metadataBuffer, metadataLength );
    std::string metadataData;
    metadataData.assign( metadataBuffer, metadataLength );
    DELETE_ARRAY( metadataBuffer );
   
    if ( !metadata.extract( metadataData ) )
    {
        ZTR_ReportError( "could not extract metadata" );
        return FALSE;
    }

    int dataLength = 0;
    inputData.read( reinterpret_cast<char *>(&dataLength), 4 );
    dataLength = ZTR_ByteSwapAsRequired_4( dataLength );

    char formatAsChar;
    inputData.read( &formatAsChar, 1 );
    dataLength--; // to account for format byte read
    if ( chunkDataPadded() )
    {
        int padBytes = ZTR_TypeNumberOfByte( getDataType() ) - 1;
        if ( padBytes > 0 )
        {
            char* bufNull = new char[4];
            inputData.read( bufNull, padBytes );
            dataLength -= padBytes;
            DELETE_ARRAY( bufNull );
        }
    }

    char* dataBuffer = new char[dataLength];
    inputData.read( dataBuffer, dataLength );
    std::string dataData;
    dataData.assign( dataBuffer, dataLength );
    DELETE_ARRAY( dataBuffer );

    ZTR_DataCompression* compression =
              ZTR_GetNewDataCompression( (int) formatAsChar );
    if ( compression == NULL )
    {
        ZTR_ReportError( "could not create compression type" );
        return FALSE;
    }

   if ( !compression->byteOrderAndUncompress( dataData, this ))
    {
        DELETE( compression );
        ZTR_ReportError( "could not uncompress data" );
        return FALSE;
    }

    DELETE( compression );

    assignedData.type = this->getDataType();
    return TRUE;
}

bool
ZTR_Chunk::readyData( const ZTR_DataCompression* compressionScheme )
{
    data.clear();
    if ( this->type() == ZTR_ChunkTypeBase )
    {
        data.append( CHUNK_BASE, 4 );
    }
    else if ( this->type() == ZTR_ChunkTypeCNF1 )
    {
        data.append( CHUNK_CNF1, 4 );
    }
    else if ( this->type() == ZTR_ChunkTypeSAMP )
    {
        data.append( CHUNK_SAMP, 4 );
    }
    else if ( this->type() == ZTR_ChunkTypeREGN )
    {
        data.append( CHUNK_REGN, 4 );
    }
    else
    {
        ZTR_ReportError( "unknown chunk type" );
        return FALSE;
    }

    std::string metadataString;
    if ( !metadata.packForZTR( &metadataString ) )
    {
        ZTR_ReportError( "could not write metadata" );
        return FALSE;
    }
    int metadataLength = metadataString.length();
    metadataLength = ZTR_ByteSwapAsRequired_4( metadataLength );
    data.append( reinterpret_cast<const char*>(&metadataLength), 4 );
    data.append( metadataString );

    std::string compressedData;
    if ( !compressionScheme->compressAndByteOrder( this,
                                                   &compressedData ))
    {    
        ZTR_ReportError( "could not compress data" );
        return FALSE;
    }

    int padBytes = 0;
    if ( chunkDataPadded() )
    {
        int bytesInType = ZTR_TypeNumberOfByte( getDataType() );
        padBytes = bytesInType - 1;
    }

    // compressed length = length + format + pad bytes
    int compressedDataLength = compressedData.length() + 1 + padBytes;
    compressedDataLength = ZTR_ByteSwapAsRequired_4( compressedDataLength );

    int format = ZTR_DataCompressionTypeToFormat(
                     compressionScheme->getDataCompressionType() );
    data.append( reinterpret_cast<const char*>(&compressedDataLength), 4 );
    data.append( reinterpret_cast<const char*>(&format), 1 );
    if ( padBytes > 0 )
    {
        char nul = '\0';
        data.append( padBytes, nul );
    }
    data.append( compressedData );

    return TRUE;
}
    
ZTR_Type
ZTR_Chunk::getDataType( void ) const
{
    ZTR_Type type = metadata.type();
    if ( type == ZTR_TypeNone )
    {
        type = defaultDataType();
    }

    return type;
}

const std::string&
ZTR_Chunk::getData( void ) const
{
    return data;
}

ZTR_Type
ZTR_Chunk::defaultDataType( void ) const
{
    return ZTR_TypeNone;
}

ZTR_MetadataList*
ZTR_Chunk::getMetadata( void )
{
    return &metadata;
}

const ZTR_MetadataList*
ZTR_Chunk::getMetadataConst( void ) const
{
    return &metadata;
}

ZTR_ChunkType
ZTR_Chunk::type( void ) const
{
    return ZTR_ChunkTypeNone;
}

bool
ZTR_Chunk::chunkDataPadded( void ) const
{
    return FALSE;
}

void
ZTR_Chunk::setUncompressedDataStr( const std::string& dataStrIn )
{
    assignedData.dataString = dataStrIn;
}

void
ZTR_Chunk::setUncompressedDataInt( const std::vector<int>& dataIntIn )
{
    assignedData.ints = dataIntIn;
}

void
ZTR_Chunk::setUncompressedDataFloat( const std::vector<float>& dataFloatIn )
{
    assignedData.floats = dataFloatIn;
}


const std::string&
ZTR_Chunk::getUncompressedDataStr( void ) const
{
    return assignedData.dataString;
}

const std::vector<int>&
ZTR_Chunk::getUncompressedDataInt( void ) const
{
    return assignedData.ints;
}

const std::vector<float>&
ZTR_Chunk::getUncompressedDataFloat( void ) const
{
    return assignedData.floats;
}

void
ZTR_Chunk::dump ( void ) const
{
    metadata.dump();
}

const ZTR_Data&
ZTR_Chunk::getAssignedData( void ) const
{
    return assignedData;
}

