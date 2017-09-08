#pragma once

#include <Windows.h>

//------------------------------------------------------------------------------
// ��ó���� ����
//------------------------------------------------------------------------------
#define BUFFER_LENGTH_MAX (1024)
#define PORT_NUMBER (3600)

//------------------------------------------------------------------------------
// �ڷ��� ����
//------------------------------------------------------------------------------
struct SocketInfo
{
    WSAOVERLAPPED overlapped;
    SOCKET socket;
    WSABUF dataBuffer;
    char packetBuffer[BUFFER_LENGTH_MAX];
    int readNumber;
    int writeNumber;
};

//------------------------------------------------------------------------------
// ���� ����, ���� �Լ� ����
//------------------------------------------------------------------------------
SOCKET listenSocket;
SOCKET clientSocket;

SOCKET CreateServerSocket(void);
void RecieveSocketEvent(SocketInfo* socketInfo);
void CALLBACK WorkerRoutine(DWORD error,
                            DWORD transferNumber, 
                            LPWSAOVERLAPPED overlapped, 
                            DWORD flags);
