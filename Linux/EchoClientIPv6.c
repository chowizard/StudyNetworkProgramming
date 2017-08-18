#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
#define IP_ADDRESS_V6 ("::1")
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
    
    int socketFD = socket(AF_INET6, SOCK_STREAM, 0);
    if(socketFD == INVALID_SOCKET)
    {
        printf("Create socket failed!\n");
        return RESULT_FAILED;
    }

    struct sockaddr_in6 serverAddress;
    memset(&serverAddress, 0x00, sizeof(struct sockaddr_in6));
    serverAddress.sin6_family = AF_INET6;
    serverAddress.sin6_flowinfo = 0;
    serverAddress.sin6_port = PORT_NUMBER;
    serverAddress.sin6_scope_id = if_nametoindex("eth0");
    
    int numericAddress = inet_pton(AF_INET6, IP_ADDRESS_V6, (void*)&serverAddress.sin6_addr);
    if(numericAddress == 0)
    {
        printf("Address convert failed!\n");
        return RESULT_FAILED;
    }

    socklen_t addressSize = sizeof(serverAddress);
    result = connect(socketFD, (const struct sockaddr*)&serverAddress, addressSize);
    if(result < 0)
    {
        printf("Connect to server failed!\n");
        return RESULT_FAILED;
    }

    char buffer[PACKET_LENGTH_MAX] = { 0 };
    while(true)
    {
        memset(buffer, 0x00, sizeof(char) * PACKET_LENGTH_MAX);

        //ssize_t readLength = read(STDIN_FILENO, buffer, PACKET_LENGTH_MAX);
        ssize_t readLength = read(STDIN_FILENO, buffer, PACKET_LENGTH_MAX);
        if(readLength <= 0)
        {
            continue;
        }

        printf("Read data : %s", buffer);

        ssize_t writeLength = write(socketFD, buffer, PACKET_LENGTH_MAX);
        if(writeLength <= 0)
        {
            printf("Write to server failed.\n");
            break;
        }

        readLength = read(socketFD, buffer, PACKET_LENGTH_MAX);
        /*if(readLength <= 0)
        {
            continue;
        }*/

        printf("-> %s", buffer);
    }

    close(socketFD);

    return RESULT_SUCCESS;
}
