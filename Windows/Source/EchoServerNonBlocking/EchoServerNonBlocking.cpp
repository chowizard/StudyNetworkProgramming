#include "stdafx.h"

#include <stdio.h>
#include <WinSock2.h>

//------------------------------------------------------------------------------
// ��ó���� ����
//------------------------------------------------------------------------------
#define PACKET_LENGTH_MAX (256)
#define PORT_NUMBER (3500)

//------------------------------------------------------------------------------
// ��ü ����
//------------------------------------------------------------------------------
struct SocketInfo
{
    SOCKET socketNumber;
    BOOL isConnect;
};

//------------------------------------------------------------------------------
// ���� ���� ����
//------------------------------------------------------------------------------
const int CLIENT_COUNT_MAX = 1024;

//------------------------------------------------------------------------------
// ��Ʈ��
//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    SocketInfo Clients[CLIENT_COUNT_MAX] = { 0 };
    memset(static_cast<void*>(Clients), 
           0, 
           sizeof(SocketInfo) * CLIENT_COUNT_MAX);

    WSADATA WindowsSocketAgent;
    int WindowsSocketStartResult = WSAStartup(WINSOCK_VERSION, 
                                              &WindowsSocketAgent);
    if(WindowsSocketStartResult != 0)
    {
        wprintf_s(L"Windows socket agent start failed!\n");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"Windows socket agent start success.\n");

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
    ZeroMemory(&ServerAddress, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
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

    // ���� �� ���� ������ ������ �����.
    u_long IOControlMode = 1;
    int SocketResult = ioctlsocket(ListenSocket, 
                                   FIONBIO, 
                                   &IOControlMode);
    if(SocketResult == SOCKET_ERROR)
    {
        wprintf_s(L"I/O control socket FIONBIO setup was failed!\n");
        return SEVERITY_ERROR;
    }

    sockaddr_in ClientAddress;
    ZeroMemory(&ClientAddress, sizeof(sockaddr_in));
    int ClientAddressLength = sizeof(sockaddr_in);
    int ClientIndex = 0;

    char Buffer[PACKET_LENGTH_MAX] = { 0 };

    while(true)
    {
        int ErrorNo = 0;

        SOCKET ClientSocket = accept(ListenSocket, 
                                     reinterpret_cast<sockaddr*>(&ClientAddress), 
                                     &ClientAddressLength);
        if(ClientSocket == INVALID_SOCKET)
        {
            ErrorNo = WSAGetLastError();
            if(ErrorNo == WSAEWOULDBLOCK)
            {
            }
            else
            {
                wprintf_s(L"Accept error!\n");
            }
        }
        else
        {
            wprintf_s(L"Client socket accept %u %d\n", ClientIndex, ClientSocket);

            // Ŭ���̾�Ʈ ������ ������ �����.
            IOControlMode = 1;
            ioctlsocket(ClientSocket, 
                        FIONBIO, 
                        &IOControlMode);

            // Ŭ���̾�Ʈ ������ ��Ͽ� ����Ѵ�.
            Clients[ClientIndex].socketNumber = ClientSocket;
            Clients[ClientIndex].isConnect = TRUE;
            ++ClientIndex;
        }

        // ������� ä���� �ִ� Ŭ���̾�Ʈ�� ���ϵ��� ����� ���ư��鼭 ���Ź޴´�.
        for(int Index = 0; Index < ClientIndex; ++Index)
        {
            if(Clients[Index].isConnect != TRUE)
            {
                continue;
            }

            ClientSocket = Clients[Index].socketNumber;

            int ReadNumber = recv(ClientSocket, 
                                  Buffer, 
                                  PACKET_LENGTH_MAX, 
                                  0);
            
            ErrorNo = WSAGetLastError();
            
            if(ReadNumber > 0)
            {
                int WriteNumber = send(ClientSocket, 
                                       Buffer, 
                                       PACKET_LENGTH_MAX,
                                       0);
            }
            else if(ErrorNo == 0)
            {
                wprintf_s(L"Client close\n");
                Clients[Index].isConnect = FALSE;
            }
            else
            {
                if(ErrorNo != WSAEWOULDBLOCK)
                {
                    wprintf_s(L"Client socket error! %d\n", ClientSocket);
                    
                    closesocket(ClientSocket);
                    Clients[Index].isConnect = FALSE;
                }
            }
        }
    }

    int ListenSocketResult = closesocket(ListenSocket);
    if(ListenSocketResult != 0)
    {
        wprintf_s(L"Listen socket agent closing failed!\n");
        return SEVERITY_ERROR;
    }

    int WindowsSocketCleanupResult = WSACleanup();
    if(WindowsSocketCleanupResult != 0)
    {
        wprintf_s(L"Windows socket agent cleanup failed!\n");
        return SEVERITY_ERROR;
    }

    return SEVERITY_SUCCESS;
}

