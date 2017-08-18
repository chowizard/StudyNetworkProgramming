#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>

//----------------------------------------------------------------------------*/
/* 전처리기 선언
//----------------------------------------------------------------------------*/
#ifndef true
    #define true (1)
#endif
#ifndef false
    #define false (0)
#endif

#define SEVERITY_SUCCESS (0)
#define SEVERITY_ERROR (1)

#define INVALID_SOCKET ((int)~0)

#define PACKET_LENGTH_MAX (1024)

//----------------------------------------------------------------------------*/
/* 전역 함수 선언
//----------------------------------------------------------------------------*/
int SetNonBlockSocket(int FileDescriptor);

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    if(argc <= 1)
    {
        printf("매개변수가 부족함!\n");
        return SEVERITY_ERROR;
    }

    int ClientFDs[1024] = { 0 };
    memset((void*)ClientFDs, 0x00, sizeof(int) * 1024);

    int ServerSocket = socket(AF_INET,
                              SOCK_STREAM,
                              IPPROTO_IP);
    if(ServerSocket < 0)
    {
        printf("Server socket create failed!\n");
        return SEVERITY_ERROR;
    }

    struct sockaddr_in ServerAddress;
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerAddress.sin_port = htons(atoi(argv[1]));
    
    int BindResult = bind(ServerSocket, 
                          (const struct sockaddr*)&ServerAddress, 
                          sizeof(ServerAddress));
    if(BindResult == INVALID_SOCKET)
    {
        printf("Server socket bind error!\n");
        return SEVERITY_ERROR;
    }

    int ListenResult = listen(ServerSocket, 5);
    if(ListenResult == INVALID_SOCKET)
    {
        printf("Server socket listen failed!\n");
        return SEVERITY_ERROR;
    }

    SetNonBlockSocket(ServerSocket);
    int MaxFD = ServerSocket;
    
    int ClientSocket = 0;
    struct sockaddr_in ClientAddress;
    int ClientAddressLength = 0;

    char Buffer[PACKET_LENGTH_MAX] = { 0 };

    while(true)
    {
        memset(Buffer, 0x00, sizeof(char) * PACKET_LENGTH_MAX);

        ClientSocket = accept(ServerSocket, 
                              (struct sockaddr*)&ClientAddress,
                              &ClientAddressLength);
        if(ClientSocket == INVALID_SOCKET)
        {
            if(errno == EAGAIN)
            {
            }
            else
            {
                continue;
            }
        }
        else
        {
            SetNonBlockSocket(ClientSocket);
            ClientFDs[ClientSocket] = 1;
            if(ClientSocket > MaxFD)
            {
                MaxFD = ClientSocket;
            }

            continue;
        }

        memset(Buffer, 0x00, sizeof(char) * PACKET_LENGTH_MAX);

        int ReadNumber = 0;
        for(int Index = 0; Index < MaxFD; ++Index)
        {
            if(ClientFDs[Index] != 1)
            {
                continue;
            }

            ClientSocket = Index;
            
            ReadNumber = read(ClientSocket, (void*)Buffer, PACKET_LENGTH_MAX);
            if(ReadNumber == -1)
            {
                if(errno == EAGAIN)
                {
                }
                else
                {
                    printf("read Error! %d\n", errno);

                    close(ClientSocket);
                    ClientFDs[Index] = -1;
                    break;
                }
            }
            else if(ReadNumber == 0)
            {
                printf("close %d\n", errno);

                close(ClientSocket);
                ClientFDs[Index] = -1;
                break;
            }
            else
            {
                printf("Read data : %s\n", Buffer);
                write(ClientSocket, Buffer, PACKET_LENGTH_MAX);
            }
        }
    }

    return SEVERITY_SUCCESS;
}

//----------------------------------------------------------------------------*/
int SetNonBlockSocket(int FileDescriptor)
{
    int Flags = fcntl(FileDescriptor, 
                      F_GETFL,
                      0);
    if(Flags == -1)
    {
        printf("File control error!\n");
        Flags = 0;
    }

    return fcntl(FileDescriptor, 
                 F_SETFL,
                 Flags | O_NONBLOCK);
}
