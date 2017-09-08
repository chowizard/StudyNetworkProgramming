#include "stdafx.h"

#include <WinSock2.h>
#include <stdio.h>

#define PORT (3600)
//#define IP ("127.0.0.1")
#define IP ("192.168.81.129")

//------------------------------------------------------------------------------
struct CalculateData
{
    int leftNumber;
    int rightNumber;
    int result;
    wchar_t operatorName;
    short error;
};

//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    CalculateData Data;

    if(argc != 4)
    {
        wprintf_s(L"Usage : %s [number 1] [number 2] [operator]\n", argv[0]);
        return SEVERITY_ERROR;
    }

    WSADATA WindowSocketAgent;
    ZeroMemory((void*)&Data, sizeof(CalculateData));
    Data.leftNumber = _wtoi(argv[1]);
    Data.rightNumber = _wtoi(argv[2]);
    Data.operatorName = argv[3][0];

    int SocketResult = WSAStartup(WINSOCK_VERSION, &WindowSocketAgent);
    if(SocketResult != 0)
    {
        return SEVERITY_ERROR;
    }

    SOCKET Socket = socket(AF_INET, 
                           SOCK_STREAM, 
                           IPPROTO_TCP);
    if(Socket == INVALID_SOCKET)
    {
        return SEVERITY_ERROR;
    }

    SOCKADDR_IN Address;
    Address.sin_family = AF_INET;
    Address.sin_port = htons(PORT);
    Address.sin_addr.S_un.S_addr = inet_addr(IP);

    int ConnectResult = connect(Socket, 
                                reinterpret_cast<sockaddr*>(&Address), 
                                sizeof(Address));
    if(ConnectResult == SOCKET_ERROR)
    {
        wprintf_s(L"Failed to connect!\n");
        return SEVERITY_ERROR;
    }

    Data.leftNumber = htonl(Data.leftNumber);
    Data.rightNumber = htonl(Data.rightNumber);
    
    // 데이터 보내기
    int SendByte = send(Socket, 
                        reinterpret_cast<const char*>(&Data), 
                        sizeof(CalculateData), 
                        0);
    if(SendByte != sizeof(CalculateData))
    {
        return SEVERITY_ERROR;
    }

    ZeroMemory(&Data, sizeof(Data));

    // 데이터 수신
    //char Buffer[256] = { 0 };
    int RecieveByte = recv(Socket, 
                           reinterpret_cast<char*>(&Data), 
                           sizeof(CalculateData), 
                           0);
    if(RecieveByte != sizeof(CalculateData))
    {
        wprintf_s(L"Recieve Data Error!");
        return SEVERITY_ERROR;
    }

    if(htons(Data.error != 0))
    {
        short ErrorCode = ntohs(Data.error);
        wprintf_s(L"Calculate Error %d\n", ErrorCode);
    }

    wprintf_s(L"%d %c %d = %d\n", 
              ntohl(Data.leftNumber), 
              Data.operatorName, 
              ntohl(Data.rightNumber),
              ntohl(Data.result));

    // Network Socket 닫기
    closesocket(Socket);
    
    // Window Socket Agent 닫기
    WSACleanup();

    return SEVERITY_SUCCESS;
}

