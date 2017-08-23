#include "stdafx.h"
#include "EchoServerOverlappedEvent.h"

#include <winsock2.h>
#include <stdio.h>
#include <windows.h>

//#if BLOCKED
//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    InitializeCriticalSection(&criticalSection);

    listenSocket = CreateServerSocket();

    DWORD ThreadID = 0;
    HANDLE threadHandle = CreateThread(NULL, 
                                       0, 
                                       ThreadProcess, 
                                       NULL, 
                                       0, 
                                       &ThreadID);
    if(threadHandle == nullptr)
    {
        wprintf_s(L"Create thread failed!\n");
        return SEVERITY_ERROR;
    }

    eventTotalCount = 1;
    
    while(true)
    {
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if(clientSocket == INVALID_SOCKET)
        {
            wprintf_s(L"Socket accept failed!\n");
            return SEVERITY_ERROR;
        }

        wprintf_s(L"Client accepted : %d\n", clientSocket);

        EnterCriticalSection(&criticalSection);
        {
            WSAEVENT socketEvent = WSACreateEvent();
            if(socketEvent != WSA_INVALID_EVENT)
            {
                windowSocketEvents[eventTotalCount] = socketEvent;
                RecieveSocketEvent(clientSocket);
            }
            else
            {
                wprintf_s(L"Window socket event cteate failed!\n");
                abort();
            }
        }
        LeaveCriticalSection(&criticalSection);
    }

    DeleteCriticalSection(&criticalSection);

    return SEVERITY_SUCCESS;
}

//------------------------------------------------------------------------------
// 서버 소켓을 생성한다.
//------------------------------------------------------------------------------
SOCKET CreateServerSocket(void)
{
    WSADATA windowSocketAgent;
    int socketStartResult = WSAStartup(WINSOCK_VERSION, &windowSocketAgent);
    if(socketStartResult != 0)
    {
        wprintf_s(L"WSA start up failed!\n");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"WSA start up success\n");

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_HOPOPTS);
    if(listenSocket == INVALID_SOCKET)
    {
        wprintf_s(L"Socket create failed!\n");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"socet create success.\n");

    sockaddr_in serverAddress;
    ZeroMemory(&serverAddress, sizeof(sockaddr_in));
    serverAddress.sin_family = PF_INET;
    serverAddress.sin_addr.S_un.S_addr = htons(INADDR_ANY);
    serverAddress.sin_port = htons(PORT_NUMBER);

    int BindResult = bind(listenSocket, 
                          reinterpret_cast<const sockaddr*>(&serverAddress),
                          sizeof(sockaddr_in));
    if(BindResult == SOCKET_ERROR)
    {
        wprintf_s(L"Socket bind failed!\n");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"Socket bind success.\n");

    int ListenResult = listen(listenSocket, 5);
    if(ListenResult == SOCKET_ERROR)
    {
        wprintf_s(L"Socket listen failed!\n");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"Socket listen success.\n");

    return listenSocket;
}

//------------------------------------------------------------------------------
// 소켓 정보를 생성한다.
//------------------------------------------------------------------------------
SocketInfo* CreateSocketInfo(SOCKET socket)
{
    SocketInfo* socketInfo = reinterpret_cast<SocketInfo*>(malloc(sizeof(SocketInfo)));
    memset(socketInfo, 0, sizeof(SocketInfo));

    socketInfos[eventTotalCount] = socketInfo;

    socketInfo->socket = socket;
    socketInfo->dataBuffer.buf = socketInfos[eventTotalCount]->packetBuffer;
    socketInfo->dataBuffer.len = BUFFER_LENGTH_MAX;
    socketInfo->overlapped.hEvent = windowSocketEvents[eventTotalCount];

    return socketInfo;
}

//------------------------------------------------------------------------------
// 소켓 이벤트 수신
//------------------------------------------------------------------------------
void RecieveSocketEvent(SOCKET acceptedSocket)
{
    SocketInfo* socketInfo = CreateSocketInfo(acceptedSocket);

    DWORD flags = 0;
    DWORD readNumber = 0;
    SocketInfo* targetSocketInfo = socketInfos[eventTotalCount];
    int recieveResult = WSARecv(targetSocketInfo->socket, 
                                &(targetSocketInfo->dataBuffer), 
                                1, 
                                &readNumber, 
                                &flags, 
                                &(targetSocketInfo->overlapped), 
                                NULL);
    if(recieveResult == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        if(errorCode != WSA_IO_PENDING)
        {
            wprintf_s(L"WSARecv is failed!\n");
            //abort();
        }
    }

    ++eventTotalCount;
}

//------------------------------------------------------------------------------
//  대상 소켓 이벤트를 제거한다.
//  제거한 소켓 이벤트의 자리는, 그 뒤 이벤트들을 앞으로 하나씩 밀어서 도로 채워
// 넣는다.
//------------------------------------------------------------------------------
void RemoveSocketEvent(int eventIndex)
{
    if(eventIndex + 1 >= eventTotalCount)
    {
        return;
    }

    for(int index = eventIndex; index < eventTotalCount; ++index)
    {
        windowSocketEvents[index] = windowSocketEvents[index + 1];
        socketInfos[index] = socketInfos[index + 1];
    }

    --eventTotalCount;
}

//------------------------------------------------------------------------------
// 대상 소켓 이벤트와 소켓 정보를 해제한다.
//------------------------------------------------------------------------------
void ReleaseSocketInfoAndEvent(WSAEVENT socketEvent, SocketInfo* socketInfo, int eventIndex)
{
    closesocket(socketInfo->socket);
    free(socketInfo);
    WSACloseEvent(socketEvent);

    EnterCriticalSection(&criticalSection);
    {
        RemoveSocketEvent(eventIndex);
    }
    LeaveCriticalSection(&criticalSection);
}

//------------------------------------------------------------------------------
// 스레드 프로세스
//------------------------------------------------------------------------------
DWORD WINAPI ThreadProcess(LPVOID argv)
{
    while(true)
    {
        DWORD eventIndex = WSAWaitForMultipleEvents(eventTotalCount, 
                                                    const_cast<const WSAEVENT*>(windowSocketEvents), 
                                                    FALSE, 
                                                    WSA_INFINITE, 
                                                    FALSE);
        if(eventIndex == WSA_WAIT_FAILED)
        {
            wprintf_s(L"WSAWaitForMultipleEvents is failed!\n");
            return SEVERITY_ERROR;
            //return SEVERITY_SUCCESS;
        }

        int targetEventIndex = eventIndex - WSA_WAIT_EVENT_0;
        SocketInfo* socketInfo = socketInfos[targetEventIndex];
        WSAEVENT socketEvent = windowSocketEvents[targetEventIndex];

        WSAResetEvent(socketEvent);

        DWORD readNumber = 0;
        DWORD flags = 0;
        BOOL overlappedResult = WSAGetOverlappedResult(socketInfo->socket, 
                                                       &(socketInfo->overlapped), 
                                                       &readNumber, 
                                                       FALSE, 
                                                       &flags);
        if((overlappedResult == FALSE) || (readNumber == 0))
        {
            ReleaseSocketInfoAndEvent(socketEvent, socketInfo, targetEventIndex);
            continue;
        }

        memset(reinterpret_cast<void*>(&socketInfo->overlapped), 0, sizeof(WSAOVERLAPPED));
        socketInfo->overlapped.hEvent = windowSocketEvents[targetEventIndex];
        socketInfo->dataBuffer.len = BUFFER_LENGTH_MAX;
        socketInfo->dataBuffer.buf = socketInfo->packetBuffer;

        send(socketInfo->socket, 
             socketInfo->packetBuffer, 
             strlen(socketInfo->packetBuffer), 
             0);
        int recieveResult = WSARecv(socketInfo->socket, 
                                    &(socketInfo->dataBuffer), 
                                    1, 
                                    &readNumber, 
                                    &flags,
                                    &(socketInfo->overlapped), 
                                    NULL);
        if(recieveResult == SOCKET_ERROR)
        {
            int errorCode = WSAGetLastError();
            if(errorCode != ERROR_IO_PENDING)
            {

            }
        }
    }

    return SEVERITY_SUCCESS;
}
//#endif


#if BLOCKED
#define MAXLINE 1024

struct SOCKETINFO
{
    WSAOVERLAPPED overlapped;
    SOCKET fd;
    WSABUF dataBuf;
    char buf[MAXLINE];
    int readn;
    int writen;
};

struct SOCKETINFO *socketArray[WSA_MAXIMUM_WAIT_EVENTS];
int EventTotal = 0;
WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
DWORD WINAPI ThreadProc(LPVOID argv);
CRITICAL_SECTION CriticalSection;

int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET listen_fd, client_fd;
    struct sockaddr_in server_addr;
    struct SOCKETINFO *sInfo;
    unsigned long ThreadId;
    unsigned long flags;
    unsigned long readn;

    //if (argc != 2)
    //{
    //    printf("Usage : %s [port number]\n", argv[0]);
    //    return 1;
    //}

    if(WSAStartup(MAKEWORD(2,2),&wsaData)!= 0)
        return 1;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd == INVALID_SOCKET)
        return 1;

    ZeroMemory(&server_addr, sizeof(struct sockaddr_in));

    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(3600);
    server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
        return 0;
    }
    if(listen(listen_fd, 5) == SOCKET_ERROR)
    {
        return 0;
    }


    if( CreateThread(NULL, 0, ThreadProc, NULL, 0, &ThreadId) == NULL)
    {
        return 1;
    }
    EventTotal = 1;
    while(1)
    {
        if(( client_fd = accept(listen_fd, NULL, NULL)) == INVALID_SOCKET)
        {
            return 1;
        }

        EnterCriticalSection(&CriticalSection);
        sInfo = (struct SOCKETINFO *)malloc(sizeof(struct SOCKETINFO));
        memset((void *)sInfo, 0x00, sizeof(struct SOCKETINFO));

        socketArray[EventTotal] = sInfo;
        sInfo->fd = client_fd;
        sInfo->dataBuf.len = MAXLINE;
        sInfo->dataBuf.buf = socketArray[EventTotal]->buf;

        EventArray[EventTotal] = WSACreateEvent( );
        sInfo->overlapped.hEvent = EventArray[EventTotal];

        flags = 0;
        WSARecv(socketArray[EventTotal]->fd, &socketArray[EventTotal]->dataBuf, 1, &readn, &flags, 
            &(socketArray[EventTotal]->overlapped), NULL);
        EventTotal++;
        LeaveCriticalSection(&CriticalSection);

    }
}

DWORD WINAPI ThreadProc(LPVOID argv)
{
    unsigned long readn;
    unsigned long index;
    unsigned long flags;
    int i;
    struct SOCKETINFO *si;

    while(1)
    {
        if(( index = WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, WSA_INFINITE, FALSE) ) == WSA_WAIT_FAILED)
        {
            return 1;
        }
        si = socketArray[index - WSA_WAIT_EVENT_0];
        WSAResetEvent(EventArray[index - WSA_WAIT_EVENT_0]);

        if( WSAGetOverlappedResult(si->fd, &(si->overlapped), &readn, FALSE, &flags) == FALSE || readn == 0)
        {
            closesocket(si->fd);
            free(si);
            WSACloseEvent(EventArray[index - WSA_WAIT_EVENT_0]);

            EnterCriticalSection(&CriticalSection);
            if(index - WSA_WAIT_EVENT_0 + 1 != EventTotal)
            {
                for(i = index - WSA_WAIT_EVENT_0; i <EventTotal; i++)
                {
                    EventArray[i] = EventArray[i+1];
                    socketArray[i] = socketArray[i+1];
                }
            }
            EventTotal --;
            LeaveCriticalSection(&CriticalSection);
            continue;
        }

        memset((void *)&si->overlapped, 0x00, sizeof(WSAOVERLAPPED));
        si->overlapped.hEvent = EventArray[index-WSA_WAIT_EVENT_0];

        si->dataBuf.len = MAXLINE;
        si->dataBuf.buf = si->buf;
        send(si->fd, si->buf, strlen(si->buf), 0);
        if( WSARecv(si->fd, &(si->dataBuf),1, &readn, &flags, &(si->overlapped), NULL) == SOCKET_ERROR)
        {
            if(WSAGetLastError( ) != ERROR_IO_PENDING)
            {
            }
        }

    }
}
#endif