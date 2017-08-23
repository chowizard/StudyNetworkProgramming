#include "stdafx.h"

#define MAX_PACKET_LENGTH (1024)

static const int PORT = 3600;
//static const char IP[] = "127.0.0.1";
static const char IP[] = "192.168.10.100";

//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    WSADATA WindowSocketAgent;
    int ResultStart = WSAStartup(WINSOCK_VERSION, &WindowSocketAgent);
    if(ResultStart != SEVERITY_SUCCESS)
    {
        wprintf_s(L"DLL load error!");
        return SEVERITY_ERROR;
    }

    SOCKET ClientSocketFD = socket(AF_INET,
                                   SOCK_STREAM, 
                                   IPPROTO_HOPOPTS);
    if(ClientSocketFD == INVALID_SOCKET)
    {
        wprintf_s(L"Socket create failure!");
        return SEVERITY_ERROR;
    }

    sockaddr_in SocketAddress;
    SocketAddress.sin_family = AF_INET;
    SocketAddress.sin_port = htons(PORT);
    SocketAddress.sin_addr.s_addr = inet_addr(IP);

    int ConnectResult = connect(ClientSocketFD, 
                                reinterpret_cast<const sockaddr*>(&SocketAddress),
                                sizeof(sockaddr));
    if(ConnectResult == SOCKET_ERROR)
    {
        wprintf_s(L"Socket connect failure!");
        WSACleanup();

        return SEVERITY_ERROR;
    }

    wprintf_s(L"Connect succeeded");

    SOCKET ClientFD = static_cast<SOCKET>(SOCKET_ERROR);
    int ClientAddressLength = 0;

    char Buffer[MAX_PACKET_LENGTH] = { 0 };

    int Count = 1;
    while(true)
    {
        // 3번 데이터를 보낼 때마다, 긴급 데이터를 전송한다.
        send(ClientFD, 
             Buffer, 
             strlen(Buffer), 
             0);
        if((Count % 3) == 0)
        {
            send(ClientFD, 
                 "9", //Buffer, 
                 1, //strlen(Buffer),
                 MSG_OOB);
            Count = 1;

            //wprintf_s(L"Send OOB Data!\n");
        }

        ++Count;
    }

    return SEVERITY_SUCCESS;
}