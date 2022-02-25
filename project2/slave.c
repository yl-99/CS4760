#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include<string.h>
#include "config.h"
#include "shared_memory.h"

int main(int argc, char *argv[])
{
    char *memBlock = attachMemoryBlock(FILENAME, BLOCKSIZE);
    if (memBlock == NULL)
    {
        fprintf(stderr, "error, no block2\n");
        return 1;
    }
    else
    {
        fprintf(stderr, "block2 OK\n");
         fprintf(stderr, "block: %s\n", memBlock);
    }

    
    for (int i = 0; i < 5; i++)
    {
        
        fprintf(stderr, "slave proc %s \n", argv[1]);
    }

    shmdt(memBlock); //detaches from shmem block
}