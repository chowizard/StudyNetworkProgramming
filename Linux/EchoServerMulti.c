#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/socket.h>

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

#define SEVERITY_SUCCESS (0)
#define SEVERITY_ERROR (1)

#define INVALID_SOCKET (-1)

#define MAX_LINE (1024)
#define PORT_NUMBER (3600)
#define SOCKET_SETSIZE (1021)

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    int ListenFD = socket(AF_INET,
                          SOCK_STREAM,
                          0);
    if(ListenFD == INVALID_SOCKET)
    {
        perror("Socket create error!\n");
        return SEVERITY_ERROR;
    }

    struct sockaddr_in ServerAddress;
    memset(&ServerAddress, 0x00, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerAddress.sin_port = htons(PORT_NUMBER);

    int BindResult = bind(ListenFD, 
                          (struct sockaddr*)&ServerAddress,
                          sizeof(ServerAddress));
    if(BindResult == INVALID_SOCKET)
    {
        perror("Socket bind error!\n");
        return SEVERITY_ERROR;
    }

    int ListenResult = listen(ListenFD, 5);
    if(ListenResult == INVALID_SOCKET)
    {
        perror("Listen error!\n");
        return SEVERITY_ERROR;
    }

    fd_set ReadFDs;
    FD_ZERO(&ReadFDs);
    FD_SET(ListenFD, &ReadFDs);

    struct sockaddr_in ClientAddress;
    socklen_t ClientAddressLength = 0;
    char Buffer[MAX_LINE] = { 0 };

    int ClientFD = 0;
    fd_set AllFDs;
    int FDNumber = 0;
    int MaxFD = ListenFD;

    while(true)
    {
        AllFDs = ReadFDs;
        printf("Select Wait\n", MaxFD);

        FDNumber = select(MaxFD + 1,
                          &AllFDs,
                          NULL,
                          NULL,
                          NULL);
        if(FD_ISSET(ListenFD, &AllFDs))
        {
            ClientAddressLength = sizeof(ClientAddress);
            ClientFD = accept(ListenFD, 
                              (struct sockaddr*)&ClientAddress, 
                              &ClientAddressLength);

            FD_SET(ClientFD, &ReadFDs);

            if(ClientFD > MaxFD)
            {
                MaxFD = ClientFD;
            }

            printf("Accept OK\n");

            continue;
        }

        for(int Index = 0; Index <= MaxFD; ++Index)
        {
            int SocketFD = Index;
            if(!FD_ISSET(SocketFD, &AllFDs))
            {
               continue; 
            }

            memset(Buffer, 0x00, MAX_LINE);
            int ReadResult = read(SocketFD, Buffer, sizeof(char) * MAX_LINE);
            if(ReadResult <= SEVERITY_SUCCESS)
            {
                close(SocketFD);
                FD_CLR(SocketFD, &ReadFDs);
            }
            else
            {
                if(0 == strncmp(Buffer, "Quit\n"))
                {
                    close(SocketFD);
                    FD_CLR(SocketFD, &ReadFDs);
                    break;
                }

                printf("Read : %s\n", Buffer);
                write(SocketFD, Buffer, sizeof(char) * strlen(Buffer));
            }

            if(--FDNumber <= 0)
            {
                break;
            }
        }
    }

    return SEVERITY_SUCCESS;
}
