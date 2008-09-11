//

#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <iterator>

#include <stddef.h>

#include "ZTR_RetrieveChunk.hh"
#include "ZTR_ChunkList.hh"
#include "SRF_File.hh"
#include "SRF_Block.hh"
#include "SRF_ContainerHeader.hh"
#include "SRF_DataBlockHeader.hh"
#include "SRF_Read.hh"

/**
 * Utility function to generate FASTQ format strings from bases and quals
 */

void
streamFastQ ( const std::string readId,
              const std::string &baseString, const ZTR_Data& qualData,
              std::ostream &os )
{

    if( baseString.find_first_not_of(".",1) >= (baseString::npos - 1) ) return;

     std::ostringstream oss;
     oss << '@' << readId << std::endl;
     oss << baseString; // basestring has embedded newline
     oss << '+' << std::endl;  // readId need not be repeated
     oss << '!'; // Padding quality value for T/G prefix

     using std::transform;
     // NB - Conversion to char is a special case, and is always allowed implicitly
     transform(qualData.ints.begin(), qualData.ints.end(),
               std::ostream_iterator<char>(oss),
               std::bind2nd( std::plus<char>(), 041)); // 041 (octal) == 33 (decimal) == '!' (ascii)

     oss << std::endl;

     // Flush string stream to provided output handle
     os << oss.str();
}


int
main(int argc, char* argv[])
{
     if ( argc < 3 )
     {
          std::cout << "Usage: reader <input file> <output file>\n";
          std::cout << "<split paired end data> - TRUE or FALSE\n";
          std::cout << "Optional arguments:\n";
          std::cout << "<delete N chars from readId start> (0 is default)\n";
          std::cout << "<append seq name from REGN to read id> (TRUE)/FALSE\n";
          exit(1);
     }

     SRF_File file( argv[1], SRF_FileOpenTypeRead );
     if ( !file.open() )
     {
          std::cout << "ERROR: Unable to open input file\n";
          exit(1);
     }

     std::string outputFileBase( argv[2] );
     std::string pairedEndStr( argv[3] );
     bool pairedEndData = FALSE;

     SRF_File* output1 = NULL;
     SRF_File* output2 = NULL;

     if ( pairedEndStr == "TRUE" )
     {
          std::cout << " * Splitting and Writing paired end reads\n";
          pairedEndData = TRUE;
          std::string out1 = outputFileBase + ".1"; // F3?
          std::string out2 = outputFileBase + ".2"; // R3?

          // FIXME - use caution before opening existing files for truncate/write         
          output1 = new SRF_File( out1.c_str(), SRF_FileOpenTypeWrite );
          output2 = new SRF_File( out2.c_str(), SRF_FileOpenTypeWrite );

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
          output1 = new SRF_File( outputFileBase.c_str(), SRF_FileOpenTypeWrite );
          if (!output1->open() )
          {
               std::cout << "ERROR: Writer can't open output file" <<
                    outputFileBase << std::endl;
               exit( -1 );
          }
     }

     // Defaults for optional parameters
     bool appendReadId = TRUE;
     int cutReadId = 0;
     if ( argc > 4 )
     {
          cutReadId = atoi( argv[4] );
          std::cout << " * Will cut " << cutReadId << " chars from each read." << std::endl;
          if ( argc > 5 )
          {
               std::string appendReadIdStr( argv[5] );
               if ( appendReadIdStr == "TRUE" )
               {
                    appendReadId = TRUE;
                    std::cout << " * Will append tag ID to reads." << std::endl;               }
          }
     }

     ZTR_ChunkMatchParams matchParamsBase, matchParamsQual;
     matchParamsBase.type = ZTR_ChunkTypeBase;
     matchParamsQual.type = ZTR_ChunkTypeCNF1;

     SRF_ContainerHeader* currContainer = NULL;
     SRF_DataBlockHeader* currDataBlockHeader = NULL;

     int readCount = 0;
     std::cout << " * Starting parse of SRF file." << std::endl;
     while ( file.getFile().peek() != EOF )
     {
          SRF_Block* block = NULL;
          SRF_BlockType blockType = SRF_GetNewBlock( file.getFile(),
                                                     ADOPT &block );

          if( blockType == SRF_BlockTypeContainerHeader ) {
               if ( currContainer != NULL )
               {
                    delete currContainer;
               }
               currContainer = (SRF_ContainerHeader*) block;

               // Verbose, diagnostic output from Container Header
               // One per file
               std::cout << " * Container Header. Format: SRF v"
                         << currContainer->getFormatVersion() << std::endl;
               std::cout << "   - Base Caller: " 
                         << currContainer->getBaseCaller() 
                         << " v" << currContainer->getBaseCallerVersion()
                         << std::endl;
          }
          else if ( blockType == SRF_BlockTypeDataBlockHeader ) {
               if ( currDataBlockHeader != NULL )
               {
                    delete currDataBlockHeader;
               }
               currDataBlockHeader = (SRF_DataBlockHeader*) block;

               // Verbose, diagnostic output from DBH. Gives idea of progress during parsing
               // Many per file
               std::cout << " * Data Block Header. Prefix: '" 
                         << currDataBlockHeader->getPrefix() 
                         << "'" << std::endl;
          }
          else if ( blockType == SRF_BlockTypeXMLBlock ) {
               std::cout << " * SRF file has XML block\n";
               //  TODO - Output xml file

          }
          else if ( blockType == SRF_BlockTypeDataBlock ) {

              if ( currContainer == NULL || currDataBlockHeader == NULL )
              {
                  // Sanity check for state machine
                  std::cerr << "SRF ERROR: Got Data Block before Container Header and Data Block Header."
                            << std::endl;
                  exit(1);
              }

               SRF_Read read( currContainer, currDataBlockHeader,
                              ABANDON (SRF_DataBlock*) block );

               readCount++;

               const std::string& ztrBlob = read.getData();
               ZTR_ChunkList chunkList;
               if ( !chunkList.extract( ztrBlob ) )
               {
                    std::cout << "ERROR: Unable to read chunks from ZTR blob\n";
                    exit(1);
               }

               ZTR_RetrieveChunk baseChunk, qualChunk;
               if ( !baseChunk.extract( chunkList,
                                        matchParamsBase,
                                        pairedEndData ) )
               {
                   std::cout << "ERROR: Unable to find base call data\n";
                   exit(1);
               }
               if ( !qualChunk.extract( chunkList,
                                        matchParamsQual,
                                        pairedEndData ) )
               {
                   std::cout << "ERROR: Unable to find quality data\n";
                    exit(1);
               }

               std::string readId = read.getReadId();
               if ( cutReadId > 0 )
               {
                    readId.erase( 0, cutReadId );
               }
               std::string readId2 = readId;

               // foreach region
               // For now, assume 1 or 2 regions, frag or mate pair

               if( appendReadId ) {
                    readId += '_' + baseChunk.getPairedEndName1();
               }
               streamFastQ(readId,
                           baseChunk.getDataForOutput(),
                           qualChunk.getData(),
                           output1->getFile());

               if ( pairedEndData )
               {
                    if( appendReadId ) {
                         readId2 += '_' + baseChunk.getPairedEndName2();
                    }
                    streamFastQ(readId2,
                                baseChunk.getData2of2ForOutput(),
                                qualChunk.getData2of2(),
                                output2->getFile());
               }
          }
          else if ( blockType == SRF_BlockTypeNone )
          {
               std::cout << "ERROR: Unable to read block\n";
          }
          else if ( blockType == SRF_BlockTypeNullIndex)
          {
               std::cout << " * SRF Null Index Block.\n";
          }
          else
          {
               std::cout << "ERROR: Unknown block type: " << blockType << "\n";
               //exit(1);  // No longer fatal, but highly unusual.
          }



          /* Verbose output. Panel numbers from DBH Prefix are better...
          if(readCount && (readCount % 100000 == 0)) {
               std::cout << " - Processed " << readCount << " reads." << std::endl;
          }
          */
          
          /* DEBUG ONLY
          if(readCount && (readCount % 100000 == 0))
          {
              std::cerr << "ABORTING EARLY FOR DEBUGGING\n";
              exit(0);
          }
          */
     }

     std::cout << " * Finished. " << std::endl;
     std::cout << " * Processed " << readCount << " reads." << std::endl;

     if ( currContainer != NULL ) delete currContainer;
     if ( currDataBlockHeader != NULL ) delete currDataBlockHeader;

     if ( output1 != NULL ) DELETE( output1 );  // This should never be null
     if ( output2 != NULL ) DELETE( output2 );

}
