//
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

#include "ZTR_ChunkCNF1.hh"

#define SCALE_KEY "SCALE"

ZTR_ChunkCNF1::ZTR_ChunkCNF1( void )
{
    scale = ZTR_ChunkCNFscalePHRED;
}
    
ZTR_ChunkCNF1::~ZTR_ChunkCNF1( void )
{
    // no-op
}

ZTR_ChunkCNFscale
ZTR_ChunkCNF1::getScale( void ) const
{
    return scale;
}

ZTR_ChunkType
ZTR_ChunkCNF1::type( void ) const
{
    return ZTR_ChunkTypeCNF1;
}

ZTR_Type
ZTR_ChunkCNF1::defaultDataType( void ) const
{
    return ZTR_Type8bitUnsignedInt;
}

bool
ZTR_ChunkCNF1::initialize( std::istream& dataIn )
{
    if ( !ZTR_Chunk::initialize( dataIn ) )
    {
        ZTR_ReportError( "could not initialize chunk" );
        return FALSE;
    }
 
    std::string key( SCALE_KEY );
    std::string value;
    if ( getMetadata()->getMetadata( key, &value ) )
    {
        if ( value == "PHRED" )
        {
            scale = ZTR_ChunkCNFscalePHRED;
        }
        else if ( value == "LO" )
        {
            scale = ZTR_ChunkCNFscaleLO;
        }
        else
        {
            ZTR_ReportError( "could not initialize chunk" );
            return FALSE;
        }
    }

    return TRUE;
}

bool
ZTR_ChunkCNF1::setup( ZTR_ChunkCNFscale scaleIn,
                      const std::vector<int>& cnfIn )
{
    std::string key( SCALE_KEY );
    std::string value;
    if ( scaleIn == ZTR_ChunkCNFscalePHRED )
    {
        value = "PHRED";
    }
    else if ( scaleIn == ZTR_ChunkCNFscaleLO )
    {
        value = "LO";
    }
    else
    {
        ZTR_ReportError( "could not set up chunk" );
        return FALSE;
    }

    getMetadata()->setMetadata( key, value );

    // QV Fixups
    std::vector<int> cnfInFixed(cnfIn);
    std::vector<int>::iterator it = cnfInFixed.begin();
    while( it != cnfInFixed.end() )
    {
        // Remap input values of 255 or larger to -1
        // Allows for round trip of QV -1 data
        if( *it == 255 ) *it = -1;
        // Cap maximum QV at 92, values above this cannot be expressed in FASTQ
        if( *it > 92 ) *it = 92;
        ++it;
    }   

    setUncompressedDataInt( cnfInFixed );

    return TRUE;
}

void
ZTR_ChunkCNF1::dump( void ) const
{
    std::cout << "CNF1 CHUNK" << std::endl;
    ZTR_Chunk::dump();

    std::cout << "DATA:";
    std::vector<int>::const_iterator it = getUncompressedDataInt().begin();

    while( it < getUncompressedDataInt().end() )
    {
        const int & v = (*it >= 255) ? -1 : *it;
        std::cout << " " << v;
        it++;
    }

    std::cout << std::endl;
}

