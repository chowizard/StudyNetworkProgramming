#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

//-----------------------------------------------------------------------------------------------*/
/* 열거형 및 상수 정의
//-----------------------------------------------------------------------------------------------*/
#define MAX_LINE (256)

//-----------------------------------------------------------------------------------------------*/
/* 엔트리 함수
//-----------------------------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    char Buffer[MAX_LINE] = { 0 };
    struct in_addr Address;
    unsigned int IPAddress = 0;

    if(argc != 2)
    {
        fprintf(stderr, 
                "Usage : %s [internet address]\n", 
                argv[0]);
        return 1;
    }

    IPAddress = inet_addr(argv[1]);
    printf("inet_addr() : %s -> %d\n", argv[1], IPAddress);
    printf("inet_ntoa() : %d -> %s\n", IPAddress, inet_ntoa(*((struct in_addr*)&IPAddress)));
  
    IPAddress = inet_aton(argv[1], &Address);
    if(IPAddress != -1)
    {
        printf("inet_aton() : %s -> %d\n", argv[1], Address.s_addr);
        printf("inet_ntoa() : %d -> %s\n", Address.s_addr, inet_ntoa(*((struct in_addr*)(&Address.s_addr))));
    }
    else
    {
        perror("Error : \n");
        return 1;
    }

    return 0;
}
