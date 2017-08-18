#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <arpa/inet.h>

#if !defined (true)
    #define true (1)
#endif
#if !defined (false)
    #define false (0)
#endif

// 성공
#define SUCCESS (0)
// 실패
#define FAILED (-1)

// 버퍼 최대 길이
#define MAX_BUFFER_SIZE 1024

//--------------------------------------------------------------------------------------*/
int ClientSocketFD;

//--------------------------------------------------------------------------------------*/
void UrgentHandler(int SignalNumber)
{
    char Buffer = 0;
    ssize_t Result = recv(ClientSocketFD, 
                          (void*)&Buffer, 
                          1,
                          MSG_OOB);
    printf("OOB Data : %c\n", Buffer);
}

//--------------------------------------------------------------------------------------*/
/* 엔트리 함수
//--------------------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("Usage : %s [Port Number]\n", argv[0]);
        return 1;
    }

    int ServerSocketFD = socket(AF_INET, 
                                SOCK_STREAM, 
                                IPPROTO_TCP);

    if(ServerSocketFD == -1)
    {
        perror("Socket creation error!\n");
        return FAILED;
    }

    struct sockaddr_in ServerAddress;
    bzero(&ServerAddress, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerAddress.sin_port = htons(atoi(argv[1]));

    int BindResult = bind(ServerSocketFD, 
                          (struct sockaddr*)&ServerAddress, 
                          sizeof(ServerAddress));
    int ListenResult = listen(ServerSocketFD, 5);

    struct sockaddr_in ClientAddress;
    int ClientLength = sizeof(ClientAddress);
    int CurrentPosition = 0;
    char Buffer[MAX_BUFFER_SIZE] = { 0 };
    while(true)
    {
        memset(Buffer, 0, MAX_BUFFER_SIZE);
        ClientSocketFD = accept(ServerSocketFD, 
                                (struct sockaddr*)&ClientAddress, 
                                &ClientLength);
        
        printf("New Client Connect : %s\n", 
               inet_ntoa(ClientAddress.sin_addr));

        signal(SIGURG, UrgentHandler);
        fcntl(ClientSocketFD, F_SETOWN, getpid());

        while(true)
        {
            CurrentPosition = read(ClientSocketFD, 
                                   Buffer, 
                                   MAX_BUFFER_SIZE);

            if(CurrentPosition <= 0)
            {
                close(ClientSocketFD);

                continue;
            }

            int WriteResult = write(ClientSocketFD, 
                                    Buffer, 
                                    MAX_BUFFER_SIZE);
            if(WriteResult <= 0)
            {
                perror("Write Error : \n");
            }
        }
        
        close(ClientSocketFD);
    }

    close(ServerSocketFD);

    return SUCCESS;
}
