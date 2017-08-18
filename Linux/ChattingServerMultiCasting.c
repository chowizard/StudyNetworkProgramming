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

    struct sockaddr_in multiCastGroup;
    memset(&multiCastGroup, 0x00, sizeof(struct sockaddr_in));
    multiCastGroup.sin_family = AF_INET;
    multiCastGroup.sin_port = htons(PORT_NUMBER);
    result = inet_pton(AF_INET, IP_ADDRESS, &multiCastGroup.sin_addr);
    if(result <= 0)
    {
        printf("Adress convert failed.\n");
        return RESULT_FAILED;
    }

    int readFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if(readFileDescriptor < 0)
    {
        printf("Create read file socket failed.\n");
        return RESULT_FAILED;
    }

    struct ip_mreq memberRequest;
    memberRequest.imr_multiaddr = multiCastGroup.sin_addr;
    memberRequest.imr_interface.s_addr = htonl(INADDR_ANY);

    result = setsockopt(readFileDescriptor, 
                        IPPROTO_IP, 
                        IP_ADD_MEMBERSHIP, 
                        &memberRequest, 
                        sizeof(memberRequest));
    if(result < RESULT_SUCCESS)
    {
        printf("Add group error.\n");
        return RESULT_FAILED;
    }

    int flag = 1;

    result = setsockopt(readFileDescriptor, 
                        SOL_SOCKET, 
                        SO_REUSEADDR, 
                        &flag, 
                        sizeof(int));
    if(result < RESULT_SUCCESS)
    {
        printf("Socket reuse address error.\n");
        return RESULT_FAILED;
    }

    result = bind(readFileDescriptor, (struct sockaddr*)&multiCastGroup, sizeof(multiCastGroup));
    if(result < 0)
    {
        printf("Socket bind failed.\n");
        return RESULT_FAILED;
    }
    
    int sendFileDesciptor = socket(AF_INET, SOCK_DGRAM, 0);
    if(sendFileDesciptor < 0)
    {
        printf("Create send file socket failed.\n");
        return RESULT_FAILED;
    }

    char message[PACKET_LENGTH_MAX] = { 0 };
    char name[16] = { 0 };

    int processID = fork();
    if(processID < 0)
    {
        printf("Process fork error.\n");
        return RESULT_FAILED;
    }
    else if(processID == 0)
    {
        while(true)
        {
            struct sockaddr_in fromAddress;
            int addressLength = sizeof(struct sockaddr_in);
            
            memset(message, 0x00, sizeof(char) * PACKET_LENGTH_MAX);
            
            result = recvfrom(readFileDescriptor, 
                              message, 
                              PACKET_LENGTH_MAX, 
                              0, 
                              (struct sockaddr*)&fromAddress, 
                              &addressLength);
            if(result < 0)
            {
                printf("Recieve error.\n");
                return RESULT_FAILED;
            }
        }
    }
    else
    {
        memset(message, 0x00, sizeof(char) * PACKET_LENGTH_MAX);
        read(0, message, PACKET_LENGTH_MAX);
        sprintf(message, "%s : %s", name, message);
        int sendLength = sendto(sendFileDesciptor, 
                                message, 
                                strlen(message), 
                                0, 
                                (struct sockaddr*)&multiCastGroup, 
                                sizeof(multiCastGroup));
        if(sendLength < strlen(message))
        {
            printf("Recieve error.\n");
            return RESULT_FAILED;
        }
    }

    return RESULT_SUCCESS;
}
