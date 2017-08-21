#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

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

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    char Buffer[MAX_LINE] = { 0 };

    if(argc != 2)
    {
        printf("Usage : %s [ socket file name ]\n", argv[0]);
        return 1;
    }

    int AccessResult = access(argv[1], F_OK);
    if(AccessResult == 0)
    {
        unlink(argv[0]);
    }

    int ServerListenFD = socket(AF_UNIX, 
                                SOCK_STREAM,
                                0);
    if(ServerListenFD == -1)
    {
        perror("Error : socket\n");
        return 1;
    }

    struct sockaddr_un ServerAddress;
    memset((void*)&ServerAddress, 0x00, sizeof(ServerAddress));
    ServerAddress.sun_family = AF_UNIX;
    strncpy(ServerAddress.sun_path,
            argv[1],
            strlen(argv[1]));

    int ResultBind = bind(ServerListenFD,
                          (struct sockaddr*)&ServerAddress,
                          sizeof(ServerAddress));
    if(ResultBind == -1)
    {
        perror("Error : bind\n");
        return 1;
    }

    // listen 함수 읽어보니까 두 번째 매개변수는 쓰지도 않는다고 함.
    int ListenResult = listen(ServerListenFD, 5);
    if(ListenResult == -1)
    {
        perror("Error : listen\n");
        return 1;
    }

    while(true)
    {
        struct sockaddr_un ClientAddress;
        memset((void*)&ClientAddress, 0x00, sizeof(ClientAddress));
        printf("accept wait\n");

        socklen_t AddressLength = 0;
        int ClientFD = accept(ServerListenFD, 
                              (struct sockaddr*)&ClientAddress,
                              &AddressLength);
        while(true)
        {
            if(ClientFD == -1)
            {
                printf("Accept Error \n");
                return 0;
            }

        memset(Buffer, 0x00, MAX_LINE);
        int ReadPosition = read(ClientFD, Buffer, MAX_LINE);
        if(ReadPosition == 0)
        {
            break;
        }
        printf("==> %s", Buffer);
        
        write(ClientFD, Buffer, strlen(Buffer));

        }
    }

    return 0;
}

