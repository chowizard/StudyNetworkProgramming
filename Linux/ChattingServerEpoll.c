#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

//----------------------------------------------------------------------------*/
/* 전처리기 선언
//----------------------------------------------------------------------------*/
#ifndef true
    #define true (1)
#endif
#ifndef false
    #define false (0)
#endif

#define SEVERITY_SUCCESS (0)
#define SEVERITY_ERROR (1)

#define SOCKET_SUCCESS (0)
#define SOCKET_ERROR (-1)

#define PORT_NUMBER (3600)
#define BUFFER_LENGTH_MAX (1024)
#define EPOLL_SIZE (20)
#define NAME_LENGTH_MAX (80)
#define USER_SIZE_MAX (1024)

//----------------------------------------------------------------------------*/
/* 전역 변수 / 함수 선언
//----------------------------------------------------------------------------*/
struct UserData
{
    int fileDescriptor;
    char name[NAME_LENGTH_MAX];
};

int UserFDMasks[USER_SIZE_MAX] = { 0 };
char Buffer[BUFFER_LENGTH_MAX] = { 0 };

int Connect(void);
int ProcessEvent(int ListenFD, 
                 int EpollFD, 
                 struct epoll_event* AllEvents, 
                 struct epoll_event* EventListenSocket);
void SendMessage(struct epoll_event* Event, char* Message);

//----------------------------------------------------------------------------*/
/* 엔트리 포인트
//----------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
    int ListenFD = Connect();
    if(ListenFD == SEVERITY_ERROR)
    {
        return SEVERITY_ERROR;
    }

    struct epoll_event* AllEvents = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
    int EpollFD = epoll_create(100);
    if(EpollFD == -1)
    {
        printf("Epoll create failed!\n");
        return SEVERITY_ERROR;
    }
    printf("Epoll create.\n");
        
    struct epoll_event EventListenSocket;
    memset(&EventListenSocket, 0x00, sizeof(struct epoll_event));
    EventListenSocket.events = EPOLLIN;
    EventListenSocket.data.fd = ListenFD;

    int EpollControlResult = epoll_ctl(EpollFD, 
                                       EPOLL_CTL_ADD, 
                                       ListenFD,
                                       &EventListenSocket);
    if(EpollControlResult == -1)
    {
        printf("ListenFD register to epoll is failed!\n");
        return SEVERITY_ERROR;
    }
    
    memset(UserFDMasks, -1, sizeof(int) * 1024);

    while(true)
    {
        int Result = ProcessEvent(ListenFD, EpollFD, AllEvents, &EventListenSocket);
        if(Result != SEVERITY_SUCCESS)
        {
            return Result;
        }
    }

    return SEVERITY_SUCCESS;
}

//----------------------------------------------------------------------------*/
// 소켓 연결
//----------------------------------------------------------------------------*/
int Connect(void)
{
    int ListenFD = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(ListenFD == SOCKET_ERROR)
    {
        printf("Socket create failed!\n");
        return SEVERITY_ERROR;
    }
    printf("Socket create success.\n");

    struct sockaddr_in SocketAddress;
    SocketAddress.sin_family = AF_INET;
    SocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    SocketAddress.sin_port = htons(PORT_NUMBER);
    int AddressLength = sizeof(SocketAddress);

    int BindResult = bind(ListenFD, (struct sockaddr*)&SocketAddress, AddressLength);
    if(BindResult == SOCKET_ERROR)
    {
        printf("Socket bind error!\n");
        return SEVERITY_ERROR;
    }
    printf("Socket bind success.\n");

    int ListenResult = listen(ListenFD, 5);
    if(ListenResult == SOCKET_ERROR)
    {
        printf("Socket listen error!\n");
        return SEVERITY_ERROR;
    }
    printf("Socket listen success.\n");

    return ListenFD;
}

//----------------------------------------------------------------------------*/
// 이벤트 처리
//----------------------------------------------------------------------------*/
int ProcessEvent(int ListenFD, 
                 int EpollFD, 
                 struct epoll_event* AllEvents, 
                 struct epoll_event* EventListenSocket)
{
    int EventCount = epoll_wait(EpollFD, 
                                AllEvents, 
                                EPOLL_SIZE, 
                                -1);
    if(EventCount == -1)
    {
        printf("Event wait error!\n");
        return SEVERITY_ERROR;
    }

    for(int Index = 0; Index < EventCount; ++Index)
    {
        struct UserData* CurrentUserData = NULL;

        if(AllEvents[Index].data.fd == ListenFD)
        {
            struct sockaddr_in ClientAddress;
            int ClientLength = sizeof(struct sockaddr);
            
            int ClientFD = accept(ListenFD, (struct sockaddr*)&ClientAddress, &ClientLength);
            if(ClientFD == SOCKET_ERROR)
            {
                printf("Accepted client file descriptor is invalid!\n");
                return SEVERITY_ERROR;
            }
            printf("Client accepted : %s\n", inet_ntoa(*((struct in_addr*)&ClientAddress)));

            UserFDMasks[ClientFD] = 1;

            CurrentUserData = (struct UserData*)malloc(sizeof(CurrentUserData));
            CurrentUserData->fileDescriptor = ClientFD;
            sprintf(CurrentUserData->name, "User (%d)", ClientFD);

            EventListenSocket->events = EPOLLIN;
            EventListenSocket->data.ptr = CurrentUserData;

            int EpollResult = epoll_ctl(EpollFD, 
                                        EPOLL_CTL_ADD, 
                                        ClientFD, 
                                        EventListenSocket);
        }
        else
        {
            CurrentUserData = AllEvents[Index].data.ptr;
            memset(Buffer, 0x00, sizeof(char) * BUFFER_LENGTH_MAX);
            
            int ReadNumber = read(CurrentUserData->fileDescriptor, Buffer, BUFFER_LENGTH_MAX);
            if(ReadNumber <= 0)
            {
                int EpollResult = epoll_ctl(EpollFD, 
                                            EPOLL_CTL_DEL, 
                                            CurrentUserData->fileDescriptor, 
                                            AllEvents);

                close(CurrentUserData->fileDescriptor);
                UserFDMasks[CurrentUserData->fileDescriptor] = -1;
                free(CurrentUserData);
            }
            else
            {
                SendMessage(&(AllEvents[Index]), Buffer);
            }
        }
    }

    return SEVERITY_SUCCESS;
}

//----------------------------------------------------------------------------*/
/* 이벤트 메시지 전송
//----------------------------------------------------------------------------*/
void SendMessage(struct epoll_event* Event, char* Message)
{
    char Buffer[BUFFER_LENGTH_MAX + 24] = { 0 };
    struct UserData* TargetUser = Event->data.ptr;
    for(int Index = 0; Index < USER_SIZE_MAX; ++Index)
    {
        if(UserFDMasks[Index] != 1)
        {
            continue;
        }
 
        memset(Buffer, 0x00, sizeof(char) * (BUFFER_LENGTH_MAX + 24));
        sprintf(Buffer, "%s %s", TargetUser->name, Message);
        printf("SendMessage : %s", Message);
        int WriteResult = write(Index, Buffer, BUFFER_LENGTH_MAX + 24);
        if(WriteResult <= 0)
        {
            printf("write result less than 0!\n");
        }
    }
}
