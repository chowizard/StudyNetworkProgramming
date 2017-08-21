#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>

//----------------------------------------------------------------------------*/
/* 전처리기 및 상수 정의
//----------------------------------------------------------------------------*/
#ifndef true
    #define true (1)
#endif
#ifndef false
    #define false (0)
#endif

#define STDIN (0)
#define MAX_LINE (80)

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    fd_set ReadFileDescriptors;
    FD_ZERO(&ReadFileDescriptors);
    FD_SET(STDIN, &ReadFileDescriptors);

    char ReadBuffer[MAX_LINE] = { 0 };
    struct timeval TimeOut;
    int FDNumber = 0;
    while(true)
    {
        TimeOut.tv_sec = 5L;
        TimeOut.tv_usec = 0L;
        printf("> ");
        fflush(stdout);

        FDNumber = select(STDIN + 1, 
                          &ReadFileDescriptors,
                          NULL,
                          NULL,
                          &TimeOut);
        if(FDNumber == 0)
        {
            printf("\nError : Time Out\n");
        }
        else
        {
            memset(ReadBuffer, 0x00, sizeof(char) * MAX_LINE);
            read(STDIN, 
                 ReadBuffer, 
                 sizeof(char) * (MAX_LINE - 1));
            
            if(0 == strncmp(ReadBuffer, "quit\n", 5))
            {
                break;
            }

            printf("Your Message : %s", ReadBuffer);
        }

        FD_SET(STDIN, &ReadFileDescriptors);
    }

    return 0;
}
