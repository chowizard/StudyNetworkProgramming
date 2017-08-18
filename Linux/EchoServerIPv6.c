#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

//------------------------------------------------------------------------------
// 전처리기 선언
//------------------------------------------------------------------------------
#ifndef false
    #define false (0)
#endif
#ifndef true
    #define true (1)
#endif

#define RESULT_SUCCESS (0)
#define RESULT_FAILED (1)

#define INVALID_SOCKET (-1)

#define PACKET_LENGTH_MAX (1024)
#define IP_ADDRESS ("127.0.0.1")
#define PORT_NUMBER (3500)

//------------------------------------------------------------------------------
// 전역 변수 / 함수 선언
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    int result = RESULT_SUCCESS;

    int listenSocketFD = socket(AF_INET6, SOCK_STREAM, 0);
    if(listenSocketFD == INVALID_SOCKET)
    {
        printf("Create socket failed!\n");
        return RESULT_FAILED;
    }

    printf("Create socket success.\n");

    struct sockaddr_in6 address;
    address.sin6_family = AF_INET6;
    address.sin6_flowinfo = 0;
    address.sin6_port = PORT_NUMBER;
    address.sin6_addr = in6addr_any;

    result = bind(listenSocketFD, (struct sockaddr*)&address, sizeof(address));
    if(result == INVALID_SOCKET)
    {
        printf("Socket bind failed!\n");
        return RESULT_FAILED;
    }

    printf("Bind socket success.\n");

    result = listen(listenSocketFD, 5);
    if(result == INVALID_SOCKET)
    {
        printf("Listen socket failed!\n");
        return RESULT_FAILED;
    }

    int clientSocketFD = 0;
    while(true)
    {
        struct sockaddr_storage clientAddress;
        socklen_t clientAddressSize = 0;
        clientSocketFD = accept(listenSocketFD, (struct sockaddr*)&clientAddress, &clientAddressSize);
        if(clientSocketFD == INVALID_SOCKET)
        {
            printf("Accepted client socket invalid!\n");
            continue;
        }

        printf("Accept Client : %d, IP Version %d %d\n", clientSocketFD, AF_INET6, clientAddress.ss_family);

        char buffer[PACKET_LENGTH_MAX] = { 0 };
        while(true)
        {
            memset(buffer, 0x00, sizeof(char) * PACKET_LENGTH_MAX);
            int readLength = read(clientSocketFD, buffer, PACKET_LENGTH_MAX);
            if(readLength <= 0)
            {
                printf("Read data from client zero.\n");
                break;
            }

            printf("Client : %d, Read data : %s", clientSocketFD, buffer);

            int writeLength = write(clientSocketFD, buffer, PACKET_LENGTH_MAX);
            if(writeLength <= 0)
            {
                printf("Echo to client failed.\n");
                break;
            }

            printf("Client : %d, Write data : %s", clientSocketFD, buffer);
        }
    }

    close(clientSocketFD);

    return RESULT_SUCCESS;
}
