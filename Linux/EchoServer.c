#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#if !defined (true)
    #define true (1)
#endif

// 버퍼 최대 길이
#define MAX_BUFFER_SIZE 1024

//--------------------------------------------------------------------------------------*/
/* 엔트리 함수
//--------------------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    int ServerSocketFD = 0;
    int ClientSocketFD = 0;
    int ClientLength = 0;
    int CurrentPosition = 0;
    char Buffer[MAX_BUFFER_SIZE] = { 0 };
    struct sockaddr_in ClientAddress;
    struct sockaddr_in ServerAddress;

    ClientLength = sizeof(ClientAddress);

    ServerSocketFD = socket(AF_INET, 
                            SOCK_STREAM, 
                            IPPROTO_TCP);

    if(ServerSocketFD == -1)
    {
        perror("Socket creation error!\n");
        return 1;
    }

    bzero(&ServerAddress, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerAddress.sin_port = htons(atoi(argv[1]));

    bind(ServerSocketFD, 
         (struct sockaddr*)&ServerAddress, 
         sizeof(ServerAddress));
    listen(ServerSocketFD, 5);

    while(true)
    {
        memset(Buffer, 0, MAX_BUFFER_SIZE);
        ClientSocketFD = accept(ServerSocketFD, 
                                (struct sockaddr*)&ClientAddress, 
                                &ClientLength);
        printf("New Client Connect : %s\n", 
               inet_ntoa(ClientAddress.sin_addr));

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

        close(ClientSocketFD);
    }

    close(ServerSocketFD);

    return 0;
}
