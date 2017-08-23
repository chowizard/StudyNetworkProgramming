#pragma once

#include <Windows.h>

//------------------------------------------------------------------------------
// ��ó���� ����
//------------------------------------------------------------------------------
#define BUFFER_LENGTH_MAX (1024)
#define PORT_NUMBER (3600)
#define INPUT_OUTPUT_HANDLE_COUNT (10)

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
HANDLE ioHandles[INPUT_OUTPUT_HANDLE_COUNT];
HANDLE completionPort;

SOCKET listenSocket;
SOCKET clientSocket;

SOCKET CreateServerSocket(sockaddr_in& serverAddress);
unsigned long WINAPI ThreadFunction(void* parameter);
