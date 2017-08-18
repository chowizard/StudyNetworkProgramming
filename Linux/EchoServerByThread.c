#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

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

#define THREAD_COUNT_MAX (2)

#define PORT_NUMBER (3600)
#define PACKET_LENGTH_MAX (1024)

//----------------------------------------------------------------------------*/
/* 전역 변수 / 함수 선언
//----------------------------------------------------------------------------*/
void* ThreadFunction(void* Data);

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    // 소켓 생성
    int ListenFD = socket(AF_INET,
                          SOCK_STREAM, 
                          0);
    if(ListenFD < 0)
    {
        printf("Socket create error!\n");
        return SEVERITY_ERROR;
    }

    printf("Socket create success.\n");

    // 소켓 바인딩
    struct sockaddr_in ServerAddress;
    memset(&ServerAddress, 0x00, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    ServerAddress.sin_port = htons(PORT_NUMBER);

    int BindResult = bind(ListenFD, 
                          (struct sockaddr*)&ServerAddress,
                          sizeof(ServerAddress));
    if(BindResult == -1)
    {
        printf("Socket bind error!\n");
        return SEVERITY_ERROR;
    }

    printf("Socket bind success.\n");

    // 소켓 수신 준비
    int ListenResut = listen(ListenFD, 5);
    if(ListenResut == -1)
    {
        printf("Socket listen error!\n");
        return SEVERITY_ERROR;
    }

    printf("Socket listen success.\n");

    // 클라이언트 소켓 
    int ClientFD = 0;
    struct sockaddr_in ClientAddress;
    socklen_t ClientAddressLength = 0;
    
    char Buffer[PACKET_LENGTH_MAX] = { 0 };
    pthread_t ThreadID = 0;
    
    while(true)
    {
        ClientAddressLength = sizeof(ClientAddress);
        ClientFD = accept(ListenFD,
                          (struct sockaddr*)&ClientAddress,
                          &ClientAddressLength);
        if(ClientFD == -1)
        {
            printf("Accept error!\n");
        }
        else
        {
            int ThreadResult = pthread_create(&ThreadID,
                                              NULL,
                                              &ThreadFunction,
                                              (void*)&ClientFD);
            pthread_detach(ThreadID);
        }
    }

    return SEVERITY_SUCCESS;
}


//----------------------------------------------------------------------------*/
void* ThreadFunction(void* Data)
{
    int SocketFD = *((int*)Data);
    struct sockaddr_in ClientAddress;
    memset(&ClientAddress, 0x00, sizeof(ClientAddress));
    socklen_t ClientAddressLength = sizeof(ClientAddress);

    int GetPeerResult = getpeername(SocketFD, 
                                    (struct sockaddr*)&ClientAddress, 
                                    &ClientAddressLength);
    if(GetPeerResult != 0)
    {
        printf("Get peer name is failed! : %d\n", GetPeerResult);
    }

    printf("Accept success! Client socket FD : %d\n", SocketFD);

    char Buffer[PACKET_LENGTH_MAX] = { 0 };
    int ReadCount = 0;
    int ReadResult = read(SocketFD, 
                          Buffer, 
                          sizeof(char) * PACKET_LENGTH_MAX);
    while(ReadResult > 0)
    {
        printf("Read data %s(%d) : %s\n", 
               inet_ntoa(ClientAddress.sin_addr),
               ntohs(ClientAddress.sin_port),
               Buffer);

        write(SocketFD, 
              Buffer,
              strlen(Buffer));
        memset(Buffer, 0x00, sizeof(Buffer));
    }

    close(SocketFD);

    printf("Worker thread end\n");

    return NULL;
}
