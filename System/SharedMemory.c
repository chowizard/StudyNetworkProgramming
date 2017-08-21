#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char** argv)
{
    key_t KeyValue = 2345;
    int SharedMemoryID = shmget(KeyValue,
                                1024,
                                IPC_CREAT | 0666);
    if(SharedMemoryID == -1)
    {
        printf("Shared memory create failure!\n");
        return 1;
    }

    printf("Shared memory create success!\n");

    return 0;
}
