#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
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
#define IP_ADDRESS_V4 ("127.0.0.1")
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

    struct addrinfo addressHints;
    memset(&addressHints, 0x00, sizeof(struct addrinfo));
    addressHints.ai_family = AF_UNSPEC;
    addressHints.ai_socktype = SOCK_STREAM;
    addressHints.ai_protocol = 0;
    addressHints.ai_flags = 0;

    struct addrinfo* resultAddressInfo = NULL;
    result = getaddrinfo(argv[1], 
                         argv[2], 
                         &addressHints, 
                         &resultAddressInfo);
    if(result != RESULT_SUCCESS)
    {
        printf("Get address information failed!\n");
        return RESULT_FAILED;
    }

    _Bool isConnected = false;
    for(struct addrinfo* currentAddressInfo = resultAddressInfo; currentAddressInfo != NULL; currentAddressInfo = currentAddressInfo->ai_next)
    {
        int socketFD = socket(currentAddressInfo->ai_family, 
                              currentAddressInfo->ai_socktype, 
                              currentAddressInfo->ai_protocol);
        if(socketFD == INVALID_SOCKET)
        {
            continue;
        }

        if(currentAddressInfo->ai_family == AF_INET6)
        {
            result = connect(socketFD, currentAddressInfo->ai_addr, currentAddressInfo->ai_addrlen);
            if(result != INVALID_SOCKET)
            {
                printf("IP version 6 connect\n");
                isConnected = true;
                break;
            }
        }
        else
        {
            result = connect(socketFD, currentAddressInfo->ai_addr, currentAddressInfo->ai_addrlen);
            if(result != INVALID_SOCKET)
            {
                printf("IP version 4 connect\n");
                isConnected = true;
                break;
            }
        }

        if(isConnected)
        {
            break;
        }

        close(socketFD);
    }

    if(!isConnected)
    {
        printf("Connection failed!\n");
        return RESULT_FAILED;
    }

    freeaddrinfo(resultAddressInfo);

    return RESULT_SUCCESS;
}
