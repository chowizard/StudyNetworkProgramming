#ifndef _GNU_SOURCE
    #define GNU_SOURCE
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/stat.h>
#include <sys/socket.h>

//----------------------------------------------------------------------------*/
// 열거형, 상수, 콜백 정의
//----------------------------------------------------------------------------*/
#ifndef false
    #define false (0)
#endif
#ifndef true
    #define true (1)
#endif

#define SEVERITY_SUCCESS (0)
#define SEVERITY_ERROR (1)

#define SOCKET_SUCCESS (1)
#define SOCKET_ERROR (-1)

#define FD_STATUS_OFF (0)
#define FD_STATUS_ON (1)

#define PORT_NUMBER (3600)
#define BUFFER_LENGTH_MAX (1024)
#define USER_COUNT_MAX (1024)

//----------------------------------------------------------------------------*/
// 전역 변수 / 함수 선언
//----------------------------------------------------------------------------*/
int UserFDs[USER_COUNT_MAX];
int ListenFD;

int Initialize(void);
int SetupSignalInputOutput(int FileDescriptor);
int MakeListner(void);
int MakeConnectedFileDescriptor(int ListenFD);
void SendChattingMessage(int TargetFD, char* Message);

//----------------------------------------------------------------------------*/
// 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char* argv)
{
    Initialize();

    ListenFD = MakeListner();
    if(ListenFD == SOCKET_ERROR)
    {
        printf("Socket listener make error!\n");
        return SEVERITY_ERROR;
    }

    siginfo_t SignalInfo;
    sigset_t SignalSet;
    sigemptyset(&SignalSet);
    sigaddset(&SignalSet, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &SignalSet, NULL);
    
    char Buffer[BUFFER_LENGTH_MAX] = { 0 };
    while(true)
    {
        int EventNumber = sigwaitinfo(&SignalSet, &SignalInfo);
        if(EventNumber == SIGRTMIN)
        {
            if(SignalInfo.si_fd == ListenFD)
            {
                int ConnectedFD = MakeConnectedFileDescriptor(ListenFD);
                if(ConnectedFD == SOCKET_ERROR)
                {
                    printf("Accept error!\n");
                }
            }
            else
            {
                memset(Buffer, 0x00, sizeof(char) * BUFFER_LENGTH_MAX);

                int ReadNumber = read(SignalInfo.si_fd, Buffer, BUFFER_LENGTH_MAX);
                if(ReadNumber <= 0)
                {
                    printf("Close user : %d", SignalInfo.si_fd);

                    UserFDs[SignalInfo.si_fd] = FD_STATUS_OFF;
                    close(SignalInfo.si_fd);
                }
                else
                {
                    SendChattingMessage(SignalInfo.si_fd, Buffer);
                }
            }
        }
    }

    return 0;
}

//----------------------------------------------------------------------------*/
// 초기화
//----------------------------------------------------------------------------*/
int Initialize(void)
{
    memset((void*)&UserFDs, 0x00, sizeof(int) * USER_COUNT_MAX);

    return 0;
}

//----------------------------------------------------------------------------*/
// 시그널 입출력을 설정한다.
//----------------------------------------------------------------------------*/
int SetupSignalInputOutput(int FileDescriptor)
{
    int Result = 0;
    
    Result = fcntl(FileDescriptor, 
                   F_SETFL, 
                   O_RDWR | O_NONBLOCK | O_ASYNC);
    if(Result < 0)
    {
        return false;
    }

    //Result = fcntl(FileDescriptor, F_SETSIG, SIGRTMIN);
    Result = fcntl(FileDescriptor, 10, SIGRTMIN);
    if(Result < 0)
    {
        return false;
    }

    int ProcessID = getpid();
    Result = fcntl(FileDescriptor, F_SETOWN, ProcessID);
    if(Result < 0)
    {
        return false;
    }
    
    return true;
}

//----------------------------------------------------------------------------*/
// 서버 측 수신자를 생성한다.
//----------------------------------------------------------------------------*/
int MakeListner(void)
{
    ListenFD = socket(AF_INET, SOCK_STREAM, INADDR_ANY);
    if(ListenFD == SOCKET_ERROR)
    {
        printf("Socket listen error!\n");
        return SOCKET_ERROR;
    }

    struct sockaddr_in ServerAddress;
    memset((void*)&ServerAddress, 0x00, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerAddress.sin_port = htons(PORT_NUMBER);

    int AddressLength = sizeof(ServerAddress);

    int BindResult = bind(ListenFD, (struct sockaddr*)&ServerAddress, AddressLength);
    if(BindResult == SOCKET_ERROR)
    {
        printf("Socket bind error!\n");
        return SOCKET_ERROR;
    }

    int ListenResult = listen(ListenFD, 5);
    if(ListenResult == SOCKET_ERROR)
    {
        printf("Socket listen error!\n");
        return SOCKET_ERROR;
    }

    int SignalResult = SetupSignalInputOutput(ListenFD);
    if(!SignalResult)
    {
        return SOCKET_ERROR;
    }

    return ListenFD;
}

//----------------------------------------------------------------------------*/
// 연결이 들어온 클라이언트 소켓을 검사해서, 연결 처리를 한다.
//----------------------------------------------------------------------------*/
int MakeConnectedFileDescriptor(int ListenFD)
{
    struct sockaddr_in ClientAddress;
    memset((void*)&ClientAddress, 0x00, sizeof(ClientAddress));
    int AddressLength = sizeof(ClientAddress);

    int ClientSocket = accept(ListenFD, (struct sockaddr*)&ClientAddress, &AddressLength);
    if(ClientSocket == SOCKET_ERROR)
    {
        return SOCKET_ERROR;
    }

    int SignalResult = SetupSignalInputOutput(ClientSocket);
    if(!SignalResult)
    {
        return SOCKET_ERROR;
    }

    UserFDs[ClientSocket] = FD_STATUS_ON;

    return ClientSocket;
}

//----------------------------------------------------------------------------*/
// 대상 클라이언트에게 채팅 메시지를 전달한다.
//----------------------------------------------------------------------------*/
void SendChattingMessage(int TargetClientFD, char* Message)
{
    char Buffer[BUFFER_LENGTH_MAX + 24] = { 0 };
    sprintf(Buffer, "User Name : (%d) %s", TargetClientFD, Message);

    int ClientFD = 0;
    for(ClientFD = 0; ClientFD < USER_COUNT_MAX; ++ClientFD)
    {
        if(UserFDs[ClientFD] == FD_STATUS_ON)
        {
            write(ClientFD, Buffer, strlen(Buffer));
        }
    }
}
