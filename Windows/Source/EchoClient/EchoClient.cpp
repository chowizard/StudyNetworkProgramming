#include "stdafx.h"

#define PACKET_LENGTH_MAX (1024)

//------------------------------------------------------------------------------
/* ���� ����
//------------------------------------------------------------------------------
const char* IP_ADDRESS = "127.0.0.1";
const int PORT_NUMBER = 3600;

//------------------------------------------------------------------------------
// ��Ʈ�� ����Ʈ
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

        // ������� �Է��� �޴´�. (���� Ű ������ �Է� ����.)
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

        // �����ڵ� ���ڿ��� �޾ұ� ������, ������ ������ ���� ���� char �迭�� ���� �� �ְ� ��ȯ�� ���ش�.
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
        
        // ������ ������ ��Ŷ �����͵��� �����ؼ� ����Ѵ�.
        // (����������, ������ char�� �޾�����, ����� wchar_t�� �ؾ� �ϹǷ� ��ȯ ������ �־�� �Ѵ�.)
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