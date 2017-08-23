#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_PACKET_LENGTH (1024)

static const int PORT = 3600;
//static const char IP[] = "127.0.0.1";
static const char IP[] = "192.168.10.100";

//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    WSADATA WindowsSocketAgent;
    int ResultStart = WSAStartup(WINSOCK_VERSION, &WindowsSocketAgent);
    if(ResultStart != SEVERITY_SUCCESS)
    {
        wprintf_s(L"DLL load error!");
        return SEVERITY_ERROR;
    }

    // 소켓 생성
    SOCKET ListenFD = socket(AF_INET, 
                             SOCK_STREAM, 
                             IPPROTO_HOPOPTS);
    if(ListenFD == INVALID_SOCKET)
    {
        wprintf_s(L"Socket create failure!");
        return SEVERITY_ERROR;
    }

    // 연결
    sockaddr_in ServerAddress;
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PORT);
    ServerAddress.sin_addr.S_un.S_addr = inet_addr(IP);

    int BindResult = bind(ListenFD, 
                          reinterpret_cast<const sockaddr*>(&ServerAddress), 
                          sizeof(ServerAddress));
    if(BindResult == SOCKET_ERROR)
    {
        wprintf_s(L"Socket bind is failure!");
        return SEVERITY_ERROR;
    }
    
    int ListenResult = listen(ListenFD, 5);
    if(ListenResult != 0)
    {
        wprintf_s(L"Socket listen is failure!");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"Server is ready");

    WSAEVENT ServerEvent = WSACreateEvent();
    int EventSelectResult = WSAEventSelect(ListenFD, 
                                           ServerEvent,
                                           FD_READ | FD_OOB);
    if(EventSelectResult == SOCKET_ERROR)
    {
        wprintf_s(L"Event select error %d\n!", WSAGetLastError());
        return SEVERITY_ERROR;
    }

    SOCKET ClientFD = INVALID_SOCKET;
    sockaddr_in ClientAddress;
    int ClientAddressLength = sizeof(ClientAddress);

    char Buffer[MAX_PACKET_LENGTH] = { 0 };
    wchar_t MessageString[(MAX_PACKET_LENGTH / 2) + 1] = { 0 };

    while(true)
    {
        ClientFD = accept(ListenFD, 
                          reinterpret_cast<struct sockaddr*>(&ClientAddress), 
                          &ClientAddressLength);
        if(ClientFD == SOCKET_ERROR)
        {
            //wprintf_s(L"Socket accept is failure!");
            continue;
        }

        /*while(true)
        {*/
            DWORD EventIndex = WSAWaitForMultipleEvents(1, 
                                                        const_cast<const HANDLE*>(&ServerEvent), 
                                                        FALSE,
                                                        WSA_INFINITE,
                                                        FALSE);
            wprintf_s(L"Read data\n");

            if(EventIndex == WSA_WAIT_FAILED)
            {
                wprintf_s(L"Event wait failed\n");
            }

            WSANETWORKEVENTS NetworkEvents;
            int EventResult = WSAEnumNetworkEvents(ListenFD,
                                                   ServerEvent,
                                                   &NetworkEvents);
            if(EventResult == SOCKET_ERROR)
            {
                wprintf_s(L"Event type error!");
            }

            if(NetworkEvents.lNetworkEvents & FD_READ)
            {
                memset(Buffer, 0x00, sizeof(char) * MAX_PACKET_LENGTH);
                recv(ListenFD, 
                     Buffer, 
                     MAX_PACKET_LENGTH - 1, 
                     0);
                ConvertPacketToWString(Buffer, 
                                       MAX_PACKET_LENGTH, 
                                       MessageString, 
                                       (MAX_PACKET_LENGTH / 2) + 1);

                wprintf_s(L"%s", MessageString);
            }

            if(NetworkEvents.lNetworkEvents & FD_OOB)
            {
                memset(Buffer, 0x00, sizeof(char) * MAX_PACKET_LENGTH);
                recv(ListenFD, 
                     Buffer, 
                     1, 
                     MSG_OOB);
                ConvertPacketToWString(Buffer, 
                                       MAX_PACKET_LENGTH, 
                                       MessageString, 
                                       (MAX_PACKET_LENGTH / 2) + 1);

                wprintf_s(L"OOB Data : %s\n", MessageString);
            }
        //}
    }

    WSACleanup();

    return SEVERITY_SUCCESS;
}