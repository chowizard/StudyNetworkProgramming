#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#ifndef true
    #define true (1)
#endif
#ifndef false
    #define false (0)
#endif

int main(int argc, char** argv)
{
    key_t SharedMemoryKey = 1234;
    int SharedMemoryID = shmget(SharedMemoryKey,
                                sizeof(int),
                                0666);
    if(SharedMemoryID == -1)
    {
        printf("Getting shared memory is failed!\n");
        return 1;
    }

    key_t SemaphoreKey = 3477;
    int SemaphoreID = semget(SemaphoreKey,
                             0,
                             0666);
    if(SemaphoreID == -1)
    {
        printf("Getting semaphore is failed!\n");
        return 1;
    }

    void* SharedMemory = shmat(SharedMemoryID,
                               NULL,
                               0);
    if(SharedMemory == (void*)-1)
    {
        printf("Getting shared memory is failed!\n");
        return 1;   
    }

    int* CalculateNumber = (int*)SharedMemory;

    struct sembuf SemaphoreOpen;
    SemaphoreOpen.sem_num = 0;
    SemaphoreOpen.sem_op = -1;
    SemaphoreOpen.sem_flg = SEM_UNDO;

    struct sembuf SemaphoreClose;
    SemaphoreClose.sem_num = 0;
    SemaphoreClose.sem_op = 1;
    SemaphoreClose.sem_flg = SEM_UNDO;

    while(true)
    {
        int SemaphoreOperateResult = semop(SemaphoreID,
                                           &SemaphoreOpen,
                                           1);
        if(SemaphoreOperateResult == -1)
        {
            return 1;
        }

        int LocalValue = (*CalculateNumber) + 1;

        sleep(1);

        (*CalculateNumber) = LocalValue;
        printf("Current Number : %d\n", *CalculateNumber);

        SemaphoreOperateResult = semop(SemaphoreID,
                                       &SemaphoreClose,
                                       1);

    }

    return 0;
}
