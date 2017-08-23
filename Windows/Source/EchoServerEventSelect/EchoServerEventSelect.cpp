#pragma once

#include "stdafx.h"

#define BUFFER_LEGNTH_MAX (1024)
#define PORT_NUMBER (3600)

//------------------------------------------------------------------------------
// 기능 : 
//------------------------------------------------------------------------------
struct SocketInfo
{
    SOCKET socket;
    char buffer[BUFFER_LEGNTH_MAX];
    int readNumber;
    int writeNumber;
};

//------------------------------------------------------------------------------
// 전역 변수 및 전역 함수 선언
//------------------------------------------------------------------------------
WSAEVENT events[WSA_MAXIMUM_WAIT_EVENTS] = { 0 };
int eventTotalCount = 0;
SocketInfo* socketInfos[WSA_MAXIMUM_WAIT_EVENTS] = { 0 };
char buffer[BUFFER_LEGNTH_MAX] = { 0 };

SOCKET CreateServerSocket(void);
int CreateSocketInfo(SOCKET socket);
void DestroySocketInfo(int eventIndex);
int ProcessNetworkEvent(WSANETWORKEVENTS* networkEvents, int eventIndex);
int ProcessNetworkEventAccept(WSANETWORKEVENTS* networkEvents, int eventIndex);
int ProcessNetworkEventRead(int eventIndex);
int ProcessNetworkEventClose(int eventIndex);

//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    SOCKET listenSocket = CreateServerSocket();

    WSAEVENT targetEvent = events[eventTotalCount - 1];
    int eventSelectResult = WSAEventSelect(listenSocket, 
                                           targetEvent,
                                           FD_ACCEPT | FD_CLOSE);
    if(eventSelectResult == SOCKET_ERROR)
    {
        wprintf_s(L"Socket bind success.\n");
        return SEVERITY_ERROR;
    }

    while(true)
    {
        int eventIndex = WSAWaitForMultipleEvents(eventTotalCount, 
                                                  events, 
                                                  FALSE, 
                                                  WSA_INFINITE, 
                                                  FALSE);
        if(eventIndex == WSA_WAIT_FAILED)
        {
            wprintf_s(L"Event wait failed!\n");
        }

        int targetIndex = eventIndex - WSA_WAIT_EVENT_0;
        SocketInfo* targetSocketInfo = socketInfos[targetIndex];
        WSAEVENT targetEvent = events[targetIndex];

        WSANETWORKEVENTS networkEvents;
        int networkEventResult = WSAEnumNetworkEvents(targetSocketInfo->socket, 
                                                      targetEvent, 
                                                      &networkEvents);
        if(networkEventResult == SOCKET_ERROR)
        {
            wprintf_s(L"Event type error!\n");
        }

        int processResult = ProcessNetworkEvent(&networkEvents, eventIndex);
        if(processResult != SEVERITY_SUCCESS)
        {
            break;
        }
    }

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

    int createSocketResult = CreateSocketInfo(listenSocket);
    if(createSocketResult == -1)
    {
        wprintf_s(L"Socket information create failed!!\n");
        return SEVERITY_ERROR;
    }

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
int CreateSocketInfo(SOCKET socket)
{
    SocketInfo* socketInfo = nullptr;

    events[eventTotalCount] = WSACreateEvent();
    if(events[eventTotalCount] == WSA_INVALID_EVENT)
    {
        wprintf_s(L"Event failure!\n");
        return -1;
    }

    socketInfo = reinterpret_cast<SocketInfo*>(malloc(sizeof(SocketInfo)));
    memset(socketInfo, 0x00, sizeof(struct SocketInfo));
    socketInfo->socket = socket;
    socketInfo->readNumber = 0;
    socketInfo->writeNumber = 0;

    socketInfos[eventTotalCount++] = socketInfo;
    
    return 0;
}

//------------------------------------------------------------------------------
// 소켓 정보를 해제한다.
//------------------------------------------------------------------------------
void DestroySocketInfo(int eventIndex)
{
    SocketInfo* socketInfo = socketInfos[eventIndex];

    closesocket(socketInfo->socket);
    free(reinterpret_cast<void*>(socketInfo));

    BOOL closeResult = WSACloseEvent(events[eventIndex]);
    if(closeResult == TRUE)
    {
        wprintf_s(L"Event close success.\n");
    }
    else
    {
        wprintf_s(L"Event close failure!\n");
    }

    for(int index = 0; index < eventTotalCount; ++index)
    {
        events[index] = events[index + 1];
        socketInfos[index] = socketInfos[index + 1];
    }

    --eventTotalCount;
}

//------------------------------------------------------------------------------
int ProcessNetworkEvent(WSANETWORKEVENTS* networkEvents, int eventIndex)
{
    if(networkEvents->lNetworkEvents & FD_ACCEPT)
    {
        int processResult = ProcessNetworkEventAccept(networkEvents, eventIndex);
        if(processResult != SEVERITY_SUCCESS)
        {
            return processResult;
        }
    }

    if(networkEvents->lNetworkEvents & FD_READ)
    {
        int processResult = ProcessNetworkEventRead(eventIndex);
        if(processResult != SEVERITY_SUCCESS)
        {
            return processResult;
        }
    }

    if(networkEvents->lNetworkEvents & FD_CLOSE)
    {
        int processResult = ProcessNetworkEventClose(eventIndex);
        if(processResult != SEVERITY_SUCCESS)
        {
            return processResult;
        }
    }

    return SEVERITY_SUCCESS;
}

//------------------------------------------------------------------------------
int ProcessNetworkEventAccept(WSANETWORKEVENTS* networkEvents, int eventIndex)
{
    int errorCodeAcceptBit = networkEvents->iErrorCode[FD_ACCEPT_BIT];
    if(errorCodeAcceptBit != 0)
    {
        return SEVERITY_ERROR;
    }

    int targetIndex = eventIndex - WSA_WAIT_EVENT_0;
    SOCKET targetSocket = socketInfos[targetIndex]->socket;
    int clientSocket = accept(targetSocket, NULL, NULL);
    if(clientSocket == INVALID_SOCKET)
    {
        return SEVERITY_ERROR;
    }

    wprintf_s(L"Client accepted : %d\n", clientSocket);

    if(eventTotalCount > WSA_MAXIMUM_WAIT_EVENTS)
    {
        wprintf_s(L"Too many connection!\n");
        closesocket(clientSocket);
    }

    CreateSocketInfo(clientSocket);

    int eventResult = WSAEventSelect(clientSocket, 
                                     events[eventTotalCount - 1], 
                                     FD_READ | FD_CLOSE);
    if(eventResult == SOCKET_ERROR)
    {
        return SEVERITY_ERROR;
    }

    return SEVERITY_SUCCESS;
}

//------------------------------------------------------------------------------
int ProcessNetworkEventRead(int eventIndex)
{
    int flags = 0;

    memset(buffer, 0x00, sizeof(char) * BUFFER_LEGNTH_MAX);

    int targetIndex = eventIndex - WSA_WAIT_EVENT_0;
    SocketInfo* socketInfo = socketInfos[targetIndex];
    socketInfo->readNumber = recv(socketInfo->socket, 
                                  socketInfo->buffer, 
                                  BUFFER_LEGNTH_MAX, 
                                  0);
    send(socketInfo->socket, 
         socketInfo->buffer, 
         socketInfo->readNumber, 
         0);

    wchar_t convertBuffer[BUFFER_LEGNTH_MAX] = { 0 };
    int bufferLength = strlen(socketInfo->buffer);
    ConvertPacketToWString(socketInfo->buffer, 
                           bufferLength, 
                           convertBuffer, 
                           BUFFER_LEGNTH_MAX);
    
    wprintf_s(L"Event data relay : Socket - %d Content - %s\n", socketInfo->socket, convertBuffer);

    return SEVERITY_SUCCESS;
}

//------------------------------------------------------------------------------
int ProcessNetworkEventClose(int eventIndex)
{
    wprintf_s(L"Socket close.\n");

    int targetIndex = eventIndex - WSA_WAIT_EVENT_0;
    DestroySocketInfo(targetIndex);

    return SEVERITY_SUCCESS;
}