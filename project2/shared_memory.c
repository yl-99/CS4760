#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include "shared_memory.h"

#define IPC_RESULT_ERROR (-1)

int getSharedBlock(int size)
{
    key_t key;

    key = ftok("./master", 12345678);
    if(key == IPC_RESULT_ERROR)
    {
        return IPC_RESULT_ERROR;
    }
    
    return shmget (key, size, 0644 | IPC_CREAT);
}

char * attachMemoryBlock(int size)
{
    int sharedBlockId = getSharedBlock(size);
    char* result;

    if(sharedBlockId == IPC_RESULT_ERROR)
    {
        return NULL;
    }

    result = shmat(sharedBlockId, NULL, 0);
    if(result == (char *)IPC_RESULT_ERROR)
    {
        return NULL;
    }

    return result;
}

bool destroyMemoryBlock(char *fileName)
{
    int sharedBlockId = getSharedBlock(0);

    if(sharedBlockId == IPC_RESULT_ERROR)
    {
        return NULL;
    }

    return (shmctl(sharedBlockId, IPC_RMID, NULL) != IPC_RESULT_ERROR);
}
