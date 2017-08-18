#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wchar.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//----------------------------------------------------------------------------*/
/* 전역 열거형 및 상수 정의
//----------------------------------------------------------------------------*/
#ifndef true
#define true (1)
#endif
#ifndef false
#define false (0)
#endif

#define PORT (3800)
#define SIZE_OF_NETWORK_RECIEVE (5)

// MS Windows의 wchar_t는 16비트지만, gcc의 wchar_t는 32비트랜다. 젠장할...
#define WChar unsigned short

// 계산 데이터
struct CalculationData
{
    int leftNumber;
    int rightNumber;
    int result;
    WChar operatorName;
    short error;
};

//----------------------------------------------------------------------------*/
/* 엔트리 함수
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    struct sockaddr_in ClientAddress;
    int ListenSocketFD = 0;
    struct sockaddr_in SocketAddress;
    int ClientSocketFD = 0;

    int AddressLength = 0;

    struct CalculationData RecordData;
    int LeftNumber = 0;
    int RightNumber = 0;
    short CalculateResult = 0;

    ListenSocketFD = socket(AF_INET, 
                            SOCK_DGRAM, 
                            0);
    if(ListenSocketFD == -1)
    {
        perror("Error : \n");
        return 1;
    }

    memset((void*)&SocketAddress, 
            0, 
            sizeof(SocketAddress));
    SocketAddress.sin_family = AF_INET;
    SocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    SocketAddress.sin_port = htons(PORT);

    int BindResult = bind(ListenSocketFD, 
                          (struct sockaddr*)&SocketAddress, 
                          sizeof(SocketAddress));
    if(BindResult == -1)
    {
        perror("Error : \n");
        return 1;
    }

    printf("서버 켰다~\n");

    while(true)
    {
        AddressLength = sizeof(ClientAddress);

        int ListenResult = recvfrom(ListenSocketFD, 
                                    (void*)&RecordData, 
                                    sizeof(RecordData), 
                                    0,
                                    (struct sockaddr*)&ClientAddress, 
                                    &AddressLength);

#if DEBUG
        printf("Client Info : %s (%d)\n", 
               inet_ntoa(ClientAddress.sin_addr),
               ntohs(ClientAddress.sin_port));
        printf("%d %c $d\n", 
               ntohl(RecordData.leftNumber), 
               RecordData.operatorName, 
               ntohl(RecordData.rightNumber));
#endif

        LeftNumber = ntohl(RecordData.leftNumber);
        RightNumber = ntohl(RecordData.rightNumber);

        switch(RecordData.operatorName)
        {
            case L'+' : 
                {
                    CalculateResult = LeftNumber + RightNumber;
                }
                break;
                
            case L'-' :
                {
                    CalculateResult = LeftNumber - RightNumber;
                }
                break;

            case L'*' : 
                {
                    CalculateResult = LeftNumber * RightNumber;
                }
                break;

            case L'/' : 
                {
                    // 0으로 나누기 문제 방지
                    if(RightNumber == 0)
                    {
                        RecordData.error = htons(2);
                        break;
                    }

                    CalculateResult = LeftNumber / RightNumber;
                }
                break;

            default :
                {
                    RecordData.error = htons(1);
                }
                break;
        }

        RecordData.result = htonl(CalculateResult);
        RecordData.error = htonl(RecordData.error);
        printf("LeftNumber : %d RightNumber : %d Result : %d Error : %d\n",
               LeftNumber, 
               RightNumber, 
               CalculateResult, 
               RecordData.error);

        sendto(ListenSocketFD, 
               (void*)&RecordData, 
               sizeof(RecordData), 
               0, 
               (struct sockaddr*)&ClientAddress, 
               AddressLength);

        close(ClientSocketFD);
    }

    close(ListenSocketFD);

    return 0;
}
