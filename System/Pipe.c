#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#ifndef true
    #define true (1)
#endif

#ifndef false
    #define false (0)
#endif

int main(int argc, char** argv)
{
    int FileDescriptor[2] = { 0 };
    int Buffer = 0;
    int i = 0;

    int PipeResult = pipe(FileDescriptor);
    if(PipeResult < 0)
    {
        perror("Pipe error!\n");
        return 1;
    }

    int ProcessID = fork();
    if(ProcessID < 0)
    {
        // 프로세스 fork 오류

        perror("Process fork failed!\n");
        return 1;
    }
    else if(ProcessID > 0)
    {
        // 부모 프로세스인 경우 처리

        close(FileDescriptor[1]);

        while(true)
        {
            read(FileDescriptor[0], 
                 (void*)&Buffer, 
                 sizeof(Buffer));
            printf("Read : %d\n", Buffer);
        }
    }
    else
    {
        // 자식 프로세스인 경우 처리
        close(FileDescriptor[0]);

        while(true)
        {
            ++i;
            write(FileDescriptor[1],
                  (void*)&i, 
                  sizeof(i));
            printf("Write : %d\n", i);
            sleep(1);
        }
    }

    return 0;
}
