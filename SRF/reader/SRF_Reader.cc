//
#include <ios>
#include <stddef.h>
#include <SRF_Block.hh>
#include <SRF_File.hh>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
        std::cout << "Unable to open input file\n";
        exit(1);
    }

    while ( file.getFile().peek() != EOF )
    {
        SRF_Block* block = NULL;
        SRF_BlockType blockType = SRF_GetNewBlock( file.getFile(),
                                                   ADOPT &block );
        if ( blockType == SRF_BlockTypeNone )
        {
            std::cout << "Unable to read block\n";
            exit(1);
        }
        if ( blockType != SRF_BlockTypeNullIndex )
        {
            block->dump();
        }
        delete block;
    }
}
