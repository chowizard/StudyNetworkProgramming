#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#ifndef true
    #define true (1)
#endif
#ifndef false
    #define false (0)
#endif

int main(int argc, char** argv)
{
    key_t MemoryKey = 1234;
    int SharedMemoryID = shmget(MemoryKey, 
                                sizeof(int),
                                IPC_CREAT | 0666);
    if(SharedMemoryID == -1)
    {
        perror("Shared memory create failure!\n");
        exit(0);
    }

    void* SharedMemory = shmat(SharedMemoryID, NULL, 0);
    if(SharedMemory == (void*)-1)
    {
        perror("Shared memory attach failure!\n");
        exit(0);
    }

    int* CalculateNumber = (int*)SharedMemory;
    while(true)
    {
        (*CalculateNumber) = (*CalculateNumber) + 2;
        sleep(1);
    }

    return 0;
}
