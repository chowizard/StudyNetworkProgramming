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

    int ResultForRead = mkfifo(ReadPath, 
                               S_IRUSR | S_IWUSR);
    int ResultForWrite = mkfifo(WritePath, 
                                S_IRUSR | S_IWUSR);

    int ReadFD = open(ReadPath,
                      O_RDWR);
    if(ReadFD == -1)
    {
        perror("ReadFD is error!\n");
        return 1;
    }

    int WriteFD = open(WritePath,
                       O_RDWR);
    if(WriteFD == -1)
    {
        perror("ReadFD is error!\n");
        return 1;
    }

    while(true)
    {
        memset(Buffer, 0x00, sizeof(char) * MAX_LINE);

        int ReadResult = read(ReadFD, Buffer, MAX_LINE);
        if(ReadResult < 0)
        {
            perror("Read error!\n");
            return 1;
        }
        printf("Read : %s\n", Buffer);

        write(WriteFD, Buffer, MAX_LINE);
        lseek(WriteFD, 0, SEEK_SET);
    }

    return 0;
}
