#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

//----------------------------------------------------------------------------*/
/* 전처리기 및 전역 설정
//----------------------------------------------------------------------------*/
#ifndef true
    #define true (1)
#endif
#ifndef false
    #define false (0)
#endif

#define MAX_LINE (1024)
#define PORT_NUMBER (3600)

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    int ServerListenFD = 0;
    int ClientFD = 0;
    pid_t ProcessID = 0;
    socklen_t AddressLength = 0;
    int ReadPosition = 0;
    char Buffer[MAX_LINE] = { 0 };
    struct sockaddr_in ServerAddress;
    struct sockaddr_in ClientAddress;
    
    ServerListenFD = socket(AF_INET, 
                            SOCK_STREAM, 
                            0);
    if(ServerListenFD < 0)
    {
        perror("Failed create socket!\n");
        return 1;
    }

    memset((void*)&ServerAddress, 
           0, 
           sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerAddress.sin_port = htons(PORT_NUMBER);

    int BindResult = bind(ServerListenFD, 
                          (struct sockaddr*)&ServerAddress, 
                          sizeof(ServerAddress));
    if(BindResult == -1)
    {
        perror("Failed bind server socket!\n");
        return 1;
    }

    int ListenResult = listen(ServerListenFD, 5);
    if(ListenResult == -1)
    {
        perror("Failed listen server socket!\n");
        return 1;
    }

    signal(SIGCHLD, SIG_IGN);

    while(true)
    {
        AddressLength = sizeof(ClientAddress);
        ClientFD = accept(ServerListenFD, 
                          (struct sockaddr*)&ClientAddress, 
                          &AddressLength);

        if(ClientFD == -1)
        {
            perror("Accept failed!\n");
            break;
        }

        ProcessID = fork();

        if(ProcessID == 0)
        {
            // 자식 프로세스인 경우의 처리
            memset(Buffer, 0x00, MAX_LINE);
            ReadPosition = read(ClientFD, 
                                Buffer, 
                                MAX_LINE);
            while(ReadPosition > 0)
            {
                printf("REad Data %s(%d) : %s/n", 
                       inet_ntoa(ClientAddress.sin_addr), 
                       ntohs(ClientAddress.sin_port),
                       Buffer);
                write(ClientFD, 
                      Buffer, 
                      strlen(Buffer));

                memset(Buffer, 0x00, MAX_LINE);
            }

            close(ClientFD);
        }
        else if(ProcessID > 0)
        {
            // 부모 프로세스인 경우의 처리
            close(ClientFD);
        }
    }

    return 0;
}
