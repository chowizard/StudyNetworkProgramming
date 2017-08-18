#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
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
// 에러
#define ERROR (1)

#define MAXLINE 1024

//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    struct sockaddr_in ClientAddress;
    int ClientLength = sizeof(ClientAddress);

    int ServerSocketFD = socket(AF_INET,
                                SOCK_STREAM,
                                0);
    if(ServerSocketFD < 0)
    {
        printf("Create socket failed!\n");
        return FAILED;
    }

    if(argc != 2)
    {
        printf("Usage : [Port Number]\n");
        return ERROR;
    }

    struct sockaddr_in ServerAddress;
    memset((void*)&ServerAddress, 0, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerAddress.sin_port = htons(atoi(argv[1]));

    bind(ServerSocketFD,
         (struct sockaddr*)&ServerAddress, 
         sizeof(ServerAddress));
    listen(ServerSocketFD, 5);

    int ClientSocketFD = 0;
    FILE* SocketFile = NULL;
    char Nickname[24] = { 0 };
    char RealName[24] = { 0 };
    int Age = 0;
    char Message[256] = { 0 };

    while(true)
    {
        ClientSocketFD = accept(ServerSocketFD,
                                (struct sockaddr*)&ClientAddress, 
                                &ClientLength);
        SocketFile = fdopen(ClientSocketFD, "r+");
        if(SocketFile == NULL)
        {
            printf("Socket file is NULL!\n");
            continue;
        }

        while(true)
        {
            fscanf(SocketFile, 
                   "%[^,],%[^,],%d,%[^\n]%*c", 
                   Nickname,
                   RealName,
                   &Age,
                   Message);
            if((feof(SocketFile) != 0) ||
               (ferror(SocketFile) != 0))
            {
                printf("Socket Close\n");
                fclose(SocketFile);
                break;
            }

            printf("Nickname = %s\n", Nickname);
            printf("Real Name = %s\n", RealName);
            printf("Age = %d\n", Age);
            printf("Message = %s\n", Message);

            fprintf(SocketFile, 
                    "Your name is %s\n"
                    "Your message is %s\n"
                    "Your birth year us %d\n",
                    RealName,
                    Message,
                    2014 - Age);
        }
    }

    return SUCCESS;
}
