//
#include <iostream>
#include <cstddef>
#include <cstdlib>

#include "SRF_Read.hh"
#include "SRF_ContainerHeader.hh"
#include "SRF_DataBlockHeader.hh"
#include "SRF_Block.hh"
#include "ZTR_ChunkList.hh"
#include "SRF_File.hh"

int
main(int argc, char* argv[])
{
    if ( argc != 2 )
    {
        std::cout << "Usage: reader <input file>\n";
        exit(1);
    }

    SRF_FileOpenType mode = SRF_FileOpenTypeRead;
    SRF_File file( argv[1], mode );
    if ( !file.open() )
    {
        std::cout << "ERROR: Unable to open input file\n";
        exit(1);
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

        if ( blockType != SRF_BlockTypeNullIndex )
        {
            block->dump();
        }

        if ( blockType == SRF_BlockTypeContainerHeader )
        {
            if ( currContainer != NULL )
            {
                delete currContainer;
            }
            // TODO write a casting method on the class
            currContainer = (SRF_ContainerHeader*) block;
        }
        else if ( blockType == SRF_BlockTypeDataBlockHeader )
        {
            if ( currDataBlockHeader != NULL )
            {
                delete currDataBlockHeader;
            }
            // TODO write a casting method on the class
            currDataBlockHeader = (SRF_DataBlockHeader*) block;
        }
        else if ( blockType == SRF_BlockTypeDataBlock )
        {
            SRF_Read read( currContainer, currDataBlockHeader,
                           ABANDON (SRF_DataBlock*) block );

            std::cout << "Full Read Id:" << read.getReadId() << std::endl;
            
            const std::string& ztrBlob = read.getData();
            ZTR_ChunkList chunkList;
            if ( !chunkList.extract( ztrBlob ) )
            {
                std::cout << "ERROR: Unable to read blob\n";
                exit(1);
            }
            chunkList.dump();
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
}
