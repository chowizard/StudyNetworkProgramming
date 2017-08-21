#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
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

//------------------------------------------------------------------------------
// 전역 변수 / 함수 선언
//------------------------------------------------------------------------------
// 수신소켓 목록
int* listenFDs = NULL;

bool CreateListenningSockets(struct addrinfo* resultAddress);

//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("Usage : %s [port]\n", argv[0]);
        return RESULT_FAILED;
    }

    int result = RESULT_SUCCESS;

    struct addrinfo addressHint;
    memset(&addressHint, 0x00, sizeof(addressHint));
    addressHint.ai_flags = AI_PASSIVE;
    addressHint.ai_family = AF_UNSPEC;
    addressHint.ai_socktype = SOCK_STREAM;

    struct addrinfo* resultAddress = NULL;
    result = getaddrinfo(NULL, 
                         argv[1], 
                         &addressHint, 
                         &resultAddress);
    if(result != RESULT_SUCCESS)
    {
        printf("Get address information failed!\n");
        return RESULT_FAILED;
    }

    printf("Get address information success.\n");

    if(!CreateListenningSockets(resultAddress))
    {
        printf("Create listen socket is failed.\n");
        return RESULT_FAILED;
    }

    struct addrinfo* currentAddress = NULL;
    int index = 0;
    for(currentAddress = resultAddress, index = 0; currentAddress != NULL; currentAddress = currentAddress->ai_next, ++index)
    {
        char buffer[80] = { 0 };

        if(currentAddress->ai_family == AF_INET)
        {
            struct sockaddr_in* socketAddress = (struct sockaddr_in*)currentAddress->ai_addr;
            inet_ntop(currentAddress->ai_family, 
                      &socketAddress->sin_addr, 
                      buffer, 
                      sizeof(buffer));
        }
        else
        {
            struct sockaddr_in6* socketAddress = (struct sockaddr_in6*)currentAddress->ai_addr;
            inet_ntop(currentAddress->ai_family, 
                      &socketAddress->sin6_addr, 
                      buffer, 
                      sizeof(buffer));
        }

        printf("<bind 정보 : Family - %d, Type - %d, Protocol - %d, Buffer - %s>\n", 
               currentAddress->ai_family, 
               currentAddress->ai_socktype, 
               currentAddress->ai_protocol, 
               buffer);

        listenFDs[index] = socket(currentAddress->ai_family, 
                                  currentAddress->ai_socktype, 
                                  currentAddress->ai_protocol);
        if(listenFDs[index] < 0)
        {
            printf("Socket create error!\n");
            continue;
        }

        printf("Socket create success.\n");

        if(currentAddress->ai_family == AF_INET6)
        {
            int option = 1;
            setsockopt(listenFDs[index], 
                       IPPROTO_IPV6, 
                       IPV6_V6ONLY, 
                       (char*)&option, 
                       sizeof(option));
        }

        result = bind(listenFDs[index], currentAddress->ai_addr, currentAddress->ai_addrlen);
        if(result != RESULT_SUCCESS)
        {
            if(errno != EADDRINUSE)
            {
                printf("Socket bind error!\n");
                return RESULT_FAILED;
            }
        }

        printf("Socket bind success.\n");

        result = listen(listenFDs[index], 5);
        if(result != RESULT_SUCCESS)
        {
            printf("Socket listen error!\n");
            return RESULT_FAILED;
        }

        printf("Socket listen success.\n");
    }

    freeaddrinfo(resultAddress);
    pause();

    return RESULT_SUCCESS;
}

//------------------------------------------------------------------------------
// 수신 소켓 목록을 생성한다.
//------------------------------------------------------------------------------
bool CreateListenningSockets(struct addrinfo* resultAddress)
{
    if(resultAddress == NULL)
    {
        printf("Get address is empty.\n");
        return false;
    }

    int listenFDCount = 0;
    for(struct addrinfo* currentAddress = resultAddress; currentAddress != NULL; currentAddress = currentAddress->ai_next)
    {
        ++listenFDCount;
    }

    if(listenFDCount == 0)
    {
        printf("Address count is zero.\n");
        return false;
    }

    listenFDs = (int*)malloc(sizeof(int) * listenFDCount);
    if(listenFDs == NULL)
    {
        return false;
    }

    printf("Address count : %d\n", listenFDCount);

    return true;
}
