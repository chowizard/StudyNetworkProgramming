#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

//----------------------------------------------------------------------------*/
/* 매크로 및 전역 변수 정의 
//----------------------------------------------------------------------------*/
#ifndef bool
    #define bool int
#endif


#ifndef true
    #define true (1)
#endif
#ifndef false
    #define false (0)
#endif

#define MAX_LINE (1024)


//----------------------------------------------------------------------------*/
/* 함수 선언
//----------------------------------------------------------------------------*/
// 1줄 읽기
void ReadLine(int MaxFD, 
              fd_set* ReadFDs);

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    if(argc != 3)
    {
        fprintf(stderr, "Usage : %s [file 1] [file2]\n", argv[0]);
        return 1;
    }

    fd_set ReadFDs;
    FD_ZERO(&ReadFDs);

    int FDList[2] = { 0 };
    int MaxFD = 0;
    for(int Index = 0; Index < 2; ++Index)
    {
        printf("Open %s\n", argv[Index + 1]);
        FDList[Index] = open(argv[Index + 1], O_RDWR);
        if(-1 == FDList[Index])
        {
            perror("Error\n");
            return 1;
        }

        if(FDList[Index] > MaxFD)
        {
            MaxFD = FDList[Index];
        }

        FD_SET(FDList[Index], &ReadFDs);
    }

    printf("Max File Desciptor is %d\n", MaxFD);

    int FDSelectResult = 0;
    while(true)
    {
        FDSelectResult = select(MaxFD + 1, 
                                &ReadFDs,
                                NULL,
                                NULL,
                                NULL);
        ReadLine(MaxFD + 1, &ReadFDs);
    }

    return 0;
}

//----------------------------------------------------------------------------*/
/* 1줄 읽기
//----------------------------------------------------------------------------*/
void ReadLine(int MaxFD,
              fd_set* ReadFDs)
{
    char Buffer[MAX_LINE] = { 0 };

    for(int Index = 0; Index < MaxFD; ++Index)
    {
        bool IsSet = FD_ISSET(Index, ReadFDs);
        if(IsSet)
        {
            while(true)
            {
                memset(Buffer, 0x00, sizeof(char) * MAX_LINE);
                int ReadResult = read(Index, 
                                      Buffer,
                                      sizeof(char) * MAX_LINE);
                if(ReadResult < (MAX_LINE - 1))
                {
                    break;
                }

                printf("(%d) -> %s\n", Index, Buffer);
            }
        }
        
        FD_SET(Index, ReadFDs);
    }
}
