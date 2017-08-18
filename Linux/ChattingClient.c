#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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

#define SOCKET_SUCCESS (0)
#define SOCKET_ERROR (-1)

#define PORT_NUMBER (3600)
#define BUFFER_LENGTH_MAX (1024)

//----------------------------------------------------------------------------*/
/* 전역 변수 / 함수 선언
//----------------------------------------------------------------------------*/
int Connect(void);

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    int SocketFD = Connect();
    if(SocketFD == SEVERITY_ERROR)
    {
        return SEVERITY_ERROR;
    }

    fd_set ReadFDSet;
    FD_ZERO(&ReadFDSet);
    FD_SET(SocketFD, &ReadFDSet);
    FD_SET(STDIN_FILENO, &ReadFDSet);

    int FileDescriptorMax = SocketFD;
    
    fd_set DefaultFDSet;
    DefaultFDSet = ReadFDSet;

    char Buffer[BUFFER_LENGTH_MAX] = { 0 };
    while(true)
    {
        // 원래 검사해야할 파일 디스크립터 셋을 복원
        ReadFDSet = DefaultFDSet;

        int FileDescriptorNumber = select(FileDescriptorMax + 1, 
                                          &ReadFDSet, 
                                          NULL, 
                                          NULL, 
                                          NULL);

        // 네트워크를 통해 받아온 문자열이 있으면, 이를 출력한다.
        if(FD_ISSET(SocketFD, &ReadFDSet))
        {
            memset(Buffer, 0x00, BUFFER_LENGTH_MAX);

            int ReadBytes = read(SocketFD, Buffer, BUFFER_LENGTH_MAX);
            if(ReadBytes <= 0)
            {
                printf("Read is error!\n");
                return SEVERITY_ERROR;
            }
            else
            {
                //printf("From socket : %s", Buffer);
            }

            int WriteBytes = write(STDOUT_FILENO, Buffer, ReadBytes);
            if(WriteBytes != ReadBytes)
            {
                printf("Write bytes count is not equal read bytes count! (WriteBytes : %d ReadBytes : %d)\n", WriteBytes, ReadBytes);
                return SEVERITY_ERROR;
            }
        }

        // 사용자의 입력을 통해 받아온 문자열이 있으면, 이를 소켓에 쓴다.
        else if(FD_ISSET(STDIN_FILENO, &ReadFDSet))
        {
            memset(Buffer, 0x00, BUFFER_LENGTH_MAX);

            int ReadBytes = read(STDIN_FILENO, Buffer, BUFFER_LENGTH_MAX);
            printf("> %s", Buffer);
            if(ReadBytes <= 0)
            {
                printf("Read is error!\n");
                return SEVERITY_ERROR;
            }

            int WriteBytes = write(SocketFD, Buffer, ReadBytes);
            if(ReadBytes != WriteBytes)
            {
                printf("Read bytes count is not equal written bytes count! (ReadBytes : %d WriteBytes : %d)\n", ReadBytes, WriteBytes);
                return SEVERITY_ERROR;
            }
        }
    }

    return SEVERITY_SUCCESS;
}

//----------------------------------------------------------------------------*/
// 소켓 연결
//----------------------------------------------------------------------------*/
int Connect(void)
{
    int SocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(SocketFD == SOCKET_ERROR)
    {
        printf("Socket create failed!\n");
        return SEVERITY_ERROR;
    }

    struct sockaddr_in SocketAddress;
    SocketAddress.sin_family = AF_INET;
    SocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    SocketAddress.sin_port = htons(PORT_NUMBER);
    int SocketLength = sizeof(SocketAddress);

    int ConnectResult = connect(SocketFD, (struct sockaddr*)&SocketAddress, SocketLength);
    if(ConnectResult == SOCKET_ERROR)
    {
        printf("Socket connect failed!\n");
        return SEVERITY_ERROR;
    }

    printf("Socket connect success : %s\n", inet_ntoa(*((struct in_addr*)&SocketAddress)));

    return SocketFD;
}

