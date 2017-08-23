#include "stdafx.h"
#include "EchoClientWSA.h"

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

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(clientSocket == INVALID_SOCKET)
    {
        wprintf_s(L"Socket create failed!");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"socet create success.\n");

    sockaddr_in connectAddress;
    ZeroMemory(&connectAddress, sizeof(sockaddr_in));
    connectAddress.sin_family = PF_INET;
    connectAddress.sin_addr.S_un.S_addr = inet_addr(connectIPAddress);
    connectAddress.sin_port = htons(PORT_NUMBER);

    int ConnectResult = connect(clientSocket, 
                                reinterpret_cast<const sockaddr*>(&connectAddress), 
                                sizeof(connectAddress));
    if(ConnectResult == SOCKET_ERROR)
    {
        wprintf_s(L"Socket connect failed!\n");

        closesocket(clientSocket);

        return SEVERITY_ERROR;
    }

    wprintf_s(L"Connect success!\n");

    char packetBuffer[PACKET_LENGTH_MAX] = { 0 };
    wchar_t inputBuffer[PACKET_LENGTH_MAX / 2] = { 0 };
    wsaBuffer.buf = packetBuffer;
    wsaBuffer.len = PACKET_LENGTH_MAX;

    DWORD flags = 0;
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

        DWORD readNumber = 0;
        DWORD writeNumber = 0;
        flags = 0;

        // �����ڵ� ���ڿ��� �޾ұ� ������, ������ ������ ���� ���� char �迭�� ���� �� �ְ� ��ȯ�� ���ش�.
        /*ConvertWStringToPacket(inputBuffer, 
                               wcslen(inputBuffer), 
                               packetBuffer, 
                               PACKET_LENGTH_MAX);*/
        ConvertPacketToWString(packetBuffer, 
                               PACKET_LENGTH_MAX, 
                               inputBuffer, 
                               wcslen(inputBuffer));
        
        WSASend(clientSocket, 
                &wsaBuffer, 
                1, 
                &writeNumber, 
                flags, 
                NULL, 
                NULL);

        ZeroMemory(packetBuffer, sizeof(wchar_t) * (PACKET_LENGTH_MAX / 2));
        
        // ������ ������ ��Ŷ �����͵��� �����ؼ� ����Ѵ�.
        // (����������, ������ char�� �޾�����, ����� wchar_t�� �ؾ� �ϹǷ� ��ȯ ������ �־�� �Ѵ�.)
        WSARecv(clientSocket, 
                &wsaBuffer, 
                1, 
                &readNumber, 
                &flags, 
                NULL, 
                NULL);

        ConvertPacketToWString(packetBuffer, 
                               PACKET_LENGTH_MAX, 
                               inputBuffer, 
                               wcslen(inputBuffer));

        wprintf_s(L"Server -> %s\n", inputBuffer);
    }

    wprintf_s(L"Connect finished!\n");

    closesocket(clientSocket);

    int SocketCleanupResult = WSACleanup();
    if(SocketCleanupResult != 0)
    {
        wprintf_s(L"WSA cleanup failed!\n");
        return SEVERITY_ERROR;
    }

    return SEVERITY_SUCCESS;
}