#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//-----------------------------------------------------------------------------------*/
/* 전역 상수 설정
//-----------------------------------------------------------------------------------*/
#ifndef true
#define true (1)
#endif

#ifndef false
#define false (0)
#endif

// Windows에서는 wchar_t가 16비트 자료형인데, Linux GCC에서는 wchar_t가 32비트 자료형이라고 함.(염병...)
#define WChar unsigned short

#define PORT_NUMBER (3800)
#define MAX_LENGTH (256)

//-----------------------------------------------------------------------------------*/
/* 전역 데이터 타입 정의
//-----------------------------------------------------------------------------------*/
struct CalculateData
{
    int leftNumber;
    int rightNumber;
    int result;
    char operatorName;
    short error;
};

//-----------------------------------------------------------------------------------*/
/* 엔트리 포인트
//-----------------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    int SocketFD = 0;
    struct sockaddr_in ClientAddress;
    struct sockaddr_in RecieveAddress;
    struct CalculateData CalculateData;

    char Message[MAX_LENGTH] = { 0 };
    socklen_t AddressLength = 0;

    int LeftNumber = 0;
    int RightNumber = 0;
    char OperatorName[2] = { 0 };

    if(argc != 2)
    {
        printf("Usage : %s [IP Address]\n", argv[0]);
        return 1;
    }

    SocketFD = socket(AF_INET,
                      SOCK_DGRAM, 
                      0);
    if(SocketFD == -1)
    {
        return 1;
    }

    memset((void*)&ClientAddress, 0, sizeof(ClientAddress));
    ClientAddress.sin_family = AF_INET;
    ClientAddress.sin_addr.s_addr = inet_addr(argv[1]);
    ClientAddress.sin_port = htons(PORT_NUMBER);

    while(true)
    {
        printf("계산할 수식을 입력하세요\n> ");

        fgets(Message, MAX_LENGTH - 1, stdin);
        printf("1\n");
        if(strncmp(Message, "Quit\n", 5) == 0)
        {
            break;
        }

        printf("여긴가?\n");
        sscanf(Message, 
               "%d %[^0-9] %d", 
               &LeftNumber, 
               OperatorName, 
               &RightNumber);

        printf("2\n");
        
        memset((void*)&CalculateData, 0, sizeof(CalculateData));
        CalculateData.leftNumber = htonl(LeftNumber);
        CalculateData.rightNumber = htonl(RightNumber);
        CalculateData.operatorName = OperatorName[0];

        printf("3\n");

        AddressLength = sizeof(ClientAddress);
        sendto(SocketFD,
               (void*)&CalculateData, 
               sizeof(CalculateData), 
               0, 
               (struct sockaddr*)&ClientAddress, 
               AddressLength);
        printf("Send~~~!\n");

        recvfrom(SocketFD, 
                 (void*)&CalculateData, 
                 sizeof(CalculateData), 
                 0, 
                 (struct sockaddr*)&RecieveAddress, 
                 &AddressLength);

        printf("%d %c %d = %d\n", 
               ntohl(CalculateData.leftNumber),
               CalculateData.operatorName, 
               ntohl(CalculateData.rightNumber), 
               ntohl(CalculateData.result));
    }

    close(SocketFD);

    return 0;
}

