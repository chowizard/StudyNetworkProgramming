#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//-----------------------------------------------------------------------------------------------*/
/* 열거형 및 상수 정의
//-----------------------------------------------------------------------------------------------*/
#define MAX_LINE (512)

//-----------------------------------------------------------------------------------------------*/
/* 엔트리 함수
//-----------------------------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    int SourceFileDescripter = 0;
    int DestinationFileDescipter = 0;
    int ReadPosition = 0;
    int TotalLength = 0;
    char Buffer[MAX_LINE] = { 0 };

    if(argc != 3)
    {
        fprintf(stderr, 
                "Usage L %s [Source File] [Destination File]\n", 
                argv[0]);
        return 1;
    }

    SourceFileDescripter = open(argv[1], O_RDONLY);
    if(SourceFileDescripter == 0)
    {
        perror("Error : \n");
        return 1;
    }

    DestinationFileDescipter = open(argv[2], 
                                    O_CREAT | O_EXCL | O_WRONLY | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(DestinationFileDescipter == 0)
    {
        perror("Error : \n");
        return 1;
    }

    if(errno == EEXIST)
    {
       perror("Error : \n");
       close(DestinationFileDescipter);

       return 1;
    }

    memset(Buffer, 0, MAX_LINE);

    do
    {
        ReadPosition = read(SourceFileDescripter, Buffer, MAX_LINE);
        if(ReadPosition <= 0)
        {
            break;
        }

        printf("ReadPosition : %d\n", ReadPosition);

        int WritePosition = write(DestinationFileDescipter, Buffer, ReadPosition);
        TotalLength += WritePosition;

        memset(Buffer, 0, sizeof(char) * MAX_LINE);
    }
    while(ReadPosition > 0);

    printf("Total copy size : %d\n", TotalLength);

    return 0;
}
