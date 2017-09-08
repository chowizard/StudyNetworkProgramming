#include "stdafx.h"

#define PACKET_LENGTH_MAX (1024)

//------------------------------------------------------------------------------
/* 전역 변수
//------------------------------------------------------------------------------
const char* IP_ADDRESS = "127.0.0.1";
const int PORT_NUMBER = 3600;

//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    char* localeValue = setlocale(LC_ALL, NULL);

    WSADATA WindowSocketAgent;
    ZeroMemory(&WindowSocketAgent, sizeof(WSADATA));

    int SocketStartupResult = WSAStartup(WINSOCK_VERSION, 
                                         &WindowSocketAgent);
    if(SocketStartupResult != 0)
    {
        wprintf_s(L"WSA startup failed!");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"WSA start up success\n");

    SOCKET ClientSocket = socket(AF_INET, 
                                 SOCK_STREAM, 
                                 IPPROTO_TCP);
    if(ClientSocket == INVALID_SOCKET)
    {
        wprintf_s(L"Socket create failed!");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"socet create success.\n");

    sockaddr_in TargetAddress;
    ZeroMemory(&TargetAddress, sizeof(sockaddr_in));
    TargetAddress.sin_family = PF_INET;
    TargetAddress.sin_addr.S_un.S_addr = inet_addr(IP_ADDRESS);
    TargetAddress.sin_port = htons(PORT_NUMBER);

    int ConnectResult = connect(ClientSocket, 
                                reinterpret_cast<const sockaddr*>(&TargetAddress), 
                                sizeof(TargetAddress));
    if(ConnectResult == SOCKET_ERROR)
    {
        wprintf_s(L"Socket connect failed!\n");

        closesocket(ClientSocket);

        return SEVERITY_ERROR;
    }

    wprintf_s(L"Connect success!\n");

    char packetBuffer[PACKET_LENGTH_MAX] = { 0 };
    wchar_t inputBuffer[PACKET_LENGTH_MAX / 2] = { 0 };

    while(true)
    {
        wprintf_s(L">");

        // 사용자의 입력을 받는다. (엔터 키 누르면 입력 끝남.)
        /*fgetws(inputBuffer, (PACKET_LENGTH_MAX / 2) - 1, stdin);
        if(0 == wcsncmp(inputBuffer, L"Quit\n", 5))
        {
            break;
        }*/
        fgets(packetBuffer, PACKET_LENGTH_MAX - 1, stdin);
        if(0 == strncmp(packetBuffer, "Quit\n", 5))
        {
            break;
        }

        // 유니코드 문자열로 받았기 때문에, 서버로 보내기 전에 먼저 char 배열에 넣을 수 있게 변환을 해준다.
        /*ConvertWStringToPacket(inputBuffer, 
                               wcslen(inputBuffer), 
                               packetBuffer, 
                               PACKET_LENGTH_MAX);*/
        ConvertPacketToWString(packetBuffer, 
                               PACKET_LENGTH_MAX, 
                               inputBuffer, 
                               wcslen(inputBuffer));
        
        int writeNumber = send(ClientSocket, 
                               const_cast<const char*>(packetBuffer), 
                               strlen(packetBuffer),
                               0);

        ZeroMemory(packetBuffer, sizeof(wchar_t) * (PACKET_LENGTH_MAX / 2));
        
        // 서버가 반향한 패킷 데이터들을 수신해서 출력한다.
        // (마찬가지로, 수신은 char로 받았지만, 출력은 wchar_t로 해야 하므로 변환 과정이 있어야 한다.)
        int readNumber = recv(ClientSocket, 
                              packetBuffer, 
                              sizeof(char) * PACKET_LENGTH_MAX, 
                              0);

        ConvertPacketToWString(packetBuffer, 
                               PACKET_LENGTH_MAX, 
                               inputBuffer, 
                               wcslen(inputBuffer));

        wprintf_s(L"Server -> %s\n", inputBuffer);
    }

    wprintf_s(L"Connect finished!\n");

    closesocket(ClientSocket);

    int SocketCleanupResult = WSACleanup();
    if(SocketCleanupResult != 0)
    {
        wprintf_s(L"WSA cleanup failed!\n");
        return SEVERITY_ERROR;
    }

    return SEVERITY_SUCCESS;
}