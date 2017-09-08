#pragma once

#include <Windows.h>

//------------------------------------------------------------------------------
// 전처리기 선언
//------------------------------------------------------------------------------
#define BUFFER_LENGTH_MAX (1024)
#define PORT_NUMBER (3600)
#define INPUT_OUTPUT_HANDLE_COUNT (10)

//------------------------------------------------------------------------------
// 자료형 선언
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
// 전역 변수, 전역 함수 선언
//------------------------------------------------------------------------------
HANDLE ioHandles[INPUT_OUTPUT_HANDLE_COUNT];
HANDLE completionPort;

SOCKET listenSocket;
SOCKET clientSocket;

SOCKET CreateServerSocket(sockaddr_in& serverAddress);
unsigned long WINAPI ThreadFunction(void* parameter);
