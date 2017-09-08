#include "stdafx.h"

#include <stdio.h>
#include <WinSock2.h>

#define PACKET_LENGTH_MAX (1024)
#define PORT_NUMBER (3500)

//------------------------------------------------------------------------------
// 전역 변수 선언
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// 전역 함수 선언
//------------------------------------------------------------------------------
DWORD WINAPI ThreadFunction(void* Data);

//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    WSADATA WindowSocketAgent;
    int WinSockStartResult = WSAStartup(WINSOCK_VERSION, 
                                        &WindowSocketAgent);
    if(WinSockStartResult != 0)
    {
        wprintf_s(L"WSA start up failed!\n");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"WSA start up success\n");

    SOCKET ListenSocket = socket(AF_INET,
                                 SOCK_STREAM,
                                 IPPROTO_HOPOPTS);
    if(ListenSocket == INVALID_SOCKET)
    {
        wprintf_s(L"Socket create failed!\n");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"socet create success.\n");

    sockaddr_in ServerAddress;
    ZeroMemory(&ServerAddress, sizeof(sockaddr_in));
    ServerAddress.sin_family = PF_INET;
    ServerAddress.sin_addr.S_un.S_addr = htons(INADDR_ANY);
    ServerAddress.sin_port = htons(PORT_NUMBER);

    int BindResult = bind(ListenSocket, 
                          reinterpret_cast<const sockaddr*>(&ServerAddress),
                          sizeof(sockaddr_in));
    if(BindResult == SOCKET_ERROR)
    {
        wprintf_s(L"Socket bind failed!\n");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"Socket bind success.\n");

    int ListenResult = listen(ListenSocket, 5);
    if(ListenResult == SOCKET_ERROR)
    {
        wprintf_s(L"Socket listen failed!\n");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"Socket listen success.\n");

    SOCKET ClientSocket = 0;
    HANDLE Thread = NULL;
    sockaddr_in ClientAddress;
    ZeroMemory(&ClientAddress, sizeof(sockaddr_in));
    int AddressLength = sizeof(sockaddr_in);

    while(true)
    {
        ClientSocket = accept(ClientSocket, 
                              reinterpret_cast<sockaddr*>(&ClientAddress),
                              &AddressLength);
        if(ClientSocket == INVALID_SOCKET)
        {
            continue;
        }
        
        Thread = CreateThread(NULL,
                              0,
                              ThreadFunction,
                              reinterpret_cast<void*>(ClientSocket),
                              0,
                              NULL);
        if(Thread == NULL)
        {
            wprintf_s(L"Thread create failed!\n");
            return SEVERITY_ERROR;
        }

        CloseHandle(Thread);
    }

    closesocket(ListenSocket);

    int WinSockEndResult = WSACleanup();
    if(WinSockEndResult != 0)
    {
        wprintf_s(L"WSA clean up failed!\n");
        return SEVERITY_ERROR;
    }

    return SEVERITY_SUCCESS;
}

//------------------------------------------------------------------------------
DWORD WINAPI ThreadFunction(void* Data)
{
    SOCKET ClientSocket = reinterpret_cast<SOCKET>(Data);
    sockaddr_in ClientAddress;
    ZeroMemory(&ClientSocket, sizeof(sockaddr_in));
    int ClientSocketLength = sizeof(sockaddr_in);

    char Buffer[PACKET_LENGTH_MAX] = { 0 };

    int ReadNumber = 0;
    while(true)
    {
        ReadNumber = recv(ClientSocket,
                          Buffer,
                          sizeof(char) * PACKET_LENGTH_MAX,
                          0);
        if(ReadNumber == 0)
        {
            break;
        }

        int GetPeerResult = getpeername(ClientSocket, 
                                        reinterpret_cast<sockaddr*>(&ClientAddress), 
                                        &ClientSocketLength);

        int WriteResult = send(ClientSocket, 
                               Buffer, 
                               sizeof(char) * PACKET_LENGTH_MAX, 
                               0);
    }

    closesocket(ClientSocket);

    wprintf_s(L"Thread close : %d\n", GetCurrentThreadId());

    return 0;
}

