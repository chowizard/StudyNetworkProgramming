#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//----------------------------------------------------------------------------*/
/* 전처리기 및 상수 정의
//----------------------------------------------------------------------------*/
#ifndef true
    #define true (1)
#endif
#ifndef false
    #define false (0)
#endif

#define MAX_LINE (1024)

const char ReadPath[] = "/tmp/myfifo_r";
const char WritePath[] = "/tmp/myfifo_w";

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    char Buffer[MAX_LINE] = { 0 };

    int ReadFD = open(ReadPath, O_RDWR);
    if(ReadFD < 0)
    {
        perror("Read open error!\n");
        return 1;
    }

    int WriteFD = open(WritePath, O_RDWR);
    if(WriteFD < 0)
    {
        perror("Write open error!\n");
        return 1;
    }

    while(true)
    {
        printf("> ");
        fflush(stdout);
        memset(Buffer, 0x00, MAX_LINE);

        int ReadResult = read(0, Buffer, MAX_LINE);
        if(ReadResult < 0)
        {
            perror("error!\n");
            return 1;
        }

        if(strncmp(Buffer, "quit\n", 5) == 0)
        {
            break;
        }

        write(WriteFD, Buffer, strlen(Buffer));
        read(ReadFD, Buffer, MAX_LINE);
        printf("Server -> %s\n", Buffer);
    }

    close(WriteFD);
    close(ReadFD);

    return 0;
}
