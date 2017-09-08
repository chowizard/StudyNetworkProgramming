#include "stdafx.h"
#include "EchoServerOverlappedCompleted.h"

#include <winsock2.h>
#include <stdio.h>
#include <windows.h>

//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    listenSocket = CreateServerSocket();

    while(true)
    {
        clientSocket = accept(listenSocket, NULL, NULL);
        if(clientSocket == INVALID_SOCKET)
        {
            return SEVERITY_ERROR;
        }

        SocketInfo* socketInfo = reinterpret_cast<SocketInfo*>(malloc(sizeof(SocketInfo)));
        memset(reinterpret_cast<void*>(socketInfo), 0, sizeof(SocketInfo));

        socketInfo->socket = clientSocket;
        socketInfo->dataBuffer.len = BUFFER_LENGTH_MAX;
        socketInfo->dataBuffer.buf = socketInfo->packetBuffer;

        RecieveSocketEvent(socketInfo);
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
// 소켓 이벤트 수신
//------------------------------------------------------------------------------
void RecieveSocketEvent(SocketInfo* socketInfo)
{
    DWORD flags = 0;
    DWORD readNumber = 0;
    int recieveResult = WSARecv(socketInfo->socket, 
                                &(socketInfo->dataBuffer), 
                                1, 
                                &readNumber, 
                                &flags, 
                                &(socketInfo->overlapped), 
                                WorkerRoutine);
    if(recieveResult == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        if(errorCode != WSA_IO_PENDING)
        {
            wprintf_s(L"WSARecv error! %d\n", errorCode);
            //abort();
        }
    }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void CALLBACK WorkerRoutine(DWORD error,
                            DWORD transferNumber, 
                            LPWSAOVERLAPPED overlapped, 
                            DWORD flags)
{
    SocketInfo* socketInfo = reinterpret_cast<SocketInfo*>(overlapped);
    if(transferNumber == 0)
    {
        closesocket(socketInfo->socket);
        free(socketInfo);
        socketInfo = nullptr;

        return;
    }

    if(socketInfo->readNumber == 0)
    {
        socketInfo->readNumber = transferNumber;
        socketInfo->writeNumber = 0;
    }
    else
    {
        socketInfo->writeNumber = transferNumber;
    }

    if(socketInfo->readNumber > socketInfo->writeNumber)
    {
        memset(reinterpret_cast<SocketInfo*>(&socketInfo->overlapped), 0, sizeof(WSAOVERLAPPED));
        socketInfo->dataBuffer.buf = socketInfo->packetBuffer + socketInfo->writeNumber;
        socketInfo->dataBuffer.len = socketInfo->readNumber - socketInfo->writeNumber;

        DWORD writeNumber = 0;
        int sendResult = WSASend(socketInfo->socket, 
                                 &(socketInfo->dataBuffer), 
                                 1, 
                                 &writeNumber, 
                                 0, 
                                 &(socketInfo->overlapped), 
                                 WorkerRoutine);
        if(sendResult == SOCKET_ERROR)
        {
            int errorCode = WSAGetLastError();
            if(errorCode != WSA_IO_PENDING)
            {
                wprintf_s(L"WSASend error!\n");
            }
        }
    }
    else
    {
        socketInfo->readNumber = 0;

        memset(reinterpret_cast<void*>(&(socketInfo->overlapped)), 0, sizeof(WSAOVERLAPPED));
        socketInfo->dataBuffer.buf = socketInfo->packetBuffer;
        socketInfo->dataBuffer.len = BUFFER_LENGTH_MAX;

        DWORD readNumber = 0;
        DWORD flags = 0;
        int receiveResult = WSARecv(socketInfo->socket, 
                                    &(socketInfo->dataBuffer), 
                                    1, 
                                    &readNumber, 
                                    &flags, 
                                    &(socketInfo->overlapped), 
                                    WorkerRoutine);
        if(receiveResult == SOCKET_ERROR)
        {
            int errorCode = WSAGetLastError();
            if(errorCode != WSA_IO_PENDING)
            {
                wprintf_s(L"WSARecv error! %d\n", errorCode);
            }
        }
    }
}