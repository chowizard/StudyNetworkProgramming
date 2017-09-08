#include "stdafx.h"
#include "EchoServerIOCP.h"

//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    sockaddr_in serverAddress;
    ZeroMemory(&serverAddress, sizeof(sockaddr_in));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.S_un.S_addr = htons(INADDR_ANY);
    serverAddress.sin_port = htons(PORT_NUMBER);

    listenSocket = CreateServerSocket(serverAddress);

    completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 
                                            NULL, 
                                            0, 
                                            0);
    if(completionPort == INVALID_HANDLE_VALUE)
    {
        wprintf_s(L"Create IOCP error!\n");
        return SEVERITY_ERROR;
    }

    wprintf_s(L"Create IOCP.\n");

    for(int count = 0; count < INPUT_OUTPUT_HANDLE_COUNT; ++count)
    {
        DWORD threadid = 0;
        ioHandles[count] = CreateThread(NULL, 
                                        0, 
                                        ThreadFunction, 
                                        NULL, 
                                        0, 
                                        &threadid);
        //ioHandles[count] = _beginthread(ThreadFunction, 0, NULL);
    }

    while(true)
    {
        sockaddr_in clientAddress;
        ZeroMemory(&clientAddress, sizeof(clientAddress));
        int addressLength = sizeof(clientAddress);
        clientSocket = accept(listenSocket, 
                              reinterpret_cast<sockaddr*>(&clientAddress), 
                              &addressLength);
        if(clientSocket == INVALID_SOCKET)
        {
            wprintf_s(L"Accept client socket failed!\n");
            return SEVERITY_ERROR;
        }

        SocketInfo* socketInfo = reinterpret_cast<SocketInfo*>(malloc(sizeof(SocketInfo)));
        socketInfo->socket = clientSocket;
        socketInfo->readNumber = 0;
        socketInfo->writeNumber = 0;
        socketInfo->dataBuffer.buf = socketInfo->packetBuffer;
        socketInfo->dataBuffer.len = BUFFER_LENGTH_MAX;

        completionPort = CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), 
                                                completionPort, 
                                                reinterpret_cast<ULONG_PTR>(socketInfo), 
                                                0);
        if(completionPort == INVALID_HANDLE_VALUE)
        {
            wprintf_s(L"Create IOCP error!\n");
            return SEVERITY_ERROR;
        }

        wprintf_s(L"Register socket to IOCP (Socket : %d)\n", static_cast<int>(socketInfo->socket));

        DWORD readNumber = 0;
        DWORD flags = 0;
        WSARecv(socketInfo->socket, 
                &socketInfo->dataBuffer, 
                1, 
                &readNumber, 
                &flags, 
                &socketInfo->overlapped, 
                NULL);
    }

    return SEVERITY_SUCCESS;
}

//------------------------------------------------------------------------------
// 서버 소켓을 생성한다.
//------------------------------------------------------------------------------
SOCKET CreateServerSocket(sockaddr_in& serverAddress)
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
unsigned long WINAPI ThreadFunction(void* parameter)
{
    DWORD flags = 0;

    while(true)
    {
        DWORD readNumber = 0;
        ULONG completionKey = NULL;
        SocketInfo* socketInfo = NULL;
        BOOL result = GetQueuedCompletionStatus(completionPort, 
                                                &readNumber, 
                                                &completionKey, 
                                                reinterpret_cast<LPOVERLAPPED*>(&socketInfo), 
                                                INFINITE);
        if(readNumber == 0)
        {
            if(socketInfo != NULL)
            {
                closesocket(socketInfo->socket);
                free(socketInfo);
            }

            continue;
        }
        else
        {
            WSASend(socketInfo->socket, 
                    &(socketInfo->dataBuffer), 
                    1, 
                    &readNumber, 
                    0, 
                    NULL, 
                    NULL);
        }

        memset(socketInfo->packetBuffer, 0, sizeof(char) * BUFFER_LENGTH_MAX);
        socketInfo->readNumber = 0;
        socketInfo->writeNumber = 0;

        WSARecv(socketInfo->socket, 
                &(socketInfo->dataBuffer), 
                1, 
                &readNumber, 
                &flags, 
                &socketInfo->overlapped, 
                NULL);
    }

    return 0;
}