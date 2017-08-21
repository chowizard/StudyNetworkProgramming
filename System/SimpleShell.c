#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

//----------------------------------------------------------------------------*/
/* 전처리기 및 전역 상수
//----------------------------------------------------------------------------*/
#ifndef true
    #define true (1)
#endif
#ifndef false
    #define false (0)
#endif

#define MAX_LINE (256)
#define PROMPT ("#  ")
#define Chop(str)\
    str[strlen(str) - 1] = 0x00;

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    char Buffer[MAX_LINE] = { 0 };
    int ProcessStatus = 0; 

    printf("My Shell Version 1.0\n");

    while(true)
    {
        printf("%s", PROMPT);
        memset(Buffer, 0, MAX_LINE);
        fgets(Buffer, MAX_LINE - 1, stdin);

        if(strncmp(Buffer, "quit\n", 5) == 0)
        {
            break;
        }
        
        Chop(Buffer);

        pid_t ChildProcessID = fork();
        if(ChildProcessID == 0)
        {
            if(execl(Buffer, Buffer, NULL) == -1)
            {
                printf("Execl failure\n");
                exit(0);
            }
        }
        
        if(ChildProcessID > 0)
        {
            printf("Child wait\n");
            wait(&ProcessStatus);
            printf("Child exit\n");
        }
    }

    return 0;
}
