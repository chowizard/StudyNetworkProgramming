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

#define MAX_BUFFER (1024)

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
    char Buffer[MAX_BUFFER] = { 0 };
    FILE* SocketFile = NULL;
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
            char* RecieveData = fgets(Buffer, 
                                      MAX_BUFFER - 1,
                                      SocketFile);
            if(RecieveData == NULL)
            {
                printf("Socket close\n");
                fclose(SocketFile);
                break;
            }

            int ResultPuts = fputs(Buffer, SocketFile);
            if(ResultPuts == FAILED)
            {
                printf("Socket close\n");
                fclose(SocketFile);
                break;
            }
        }
    }

    return SUCCESS;
}
