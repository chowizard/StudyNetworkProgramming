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
    if(AccessResult == -1)
    {
        printf("socket file access error\n");
        return 1;
    }

    int SocketFD = socket(AF_UNIX,
                          SOCK_STREAM, 
                          0);

    struct sockaddr_un SocketAddress;
    memset((void*)&SocketAddress, 0x00, sizeof(SocketAddress));
    SocketAddress.sun_family = AF_UNIX;
    strncpy(SocketAddress.sun_path, argv[1], strlen(argv[1]));

    int ClientLength = sizeof(SocketAddress);

    int ConnectResult = connect(SocketFD, 
                                (struct sockaddr*)&SocketAddress,
                                ClientLength);
    if(ConnectResult == -1)
    {
        printf("Connection failed!\n");
        return 1;
    }

    while(true)
    {
        memset(Buffer, 0x00, MAX_LINE);
        read(0, Buffer, MAX_LINE);
        if(strncmp(Buffer, "quit\n", 5) == 0)
        {
            break;
        }

        write(SocketFD, Buffer, strlen(Buffer));
        read(SocketFD, Buffer, MAX_LINE);
        printf("Server -> %s\n", Buffer);
    }

    return 0;
}
