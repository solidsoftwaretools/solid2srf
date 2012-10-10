//
#include <iostream>
#include <string>
#include <cstdlib>

#include "SRF_Block.hh"
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
        std::cerr << "Unable to open input file\n";
        exit(1);
    }

    while ( file.getFile().peek() != EOF )
    {
        SRF_Block* block = NULL;
        SRF_BlockType blockType = SRF_GetNewBlock( file.getFile(),
                                                   ADOPT &block );
        if ( blockType == SRF_BlockTypeNone )
        {
            std::cerr << "Unable to read block\n";
            exit(1);
        }
        if ( blockType != SRF_BlockTypeNullIndex )
        {
            block->dump();
        }
        delete block;
    }
}
