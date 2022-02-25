#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>

char *attachMemoryBlock(char *fileName, int size);
bool destroyMemoryBlock(char *fileName);

#define BLOCKSIZE 4096
#define FILENAME "write_shmem.c"

#endif