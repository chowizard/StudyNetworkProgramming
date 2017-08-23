#pragma once

#include <Windows.h>

//------------------------------------------------------------------------------
// 전처리기 선언
//------------------------------------------------------------------------------
#define BUFFER_LENGTH_MAX (1024)
#define PORT_NUMBER (3600)

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
SOCKET listenSocket;
SOCKET clientSocket;

SOCKET CreateServerSocket(void);
void RecieveSocketEvent(SocketInfo* socketInfo);
void CALLBACK WorkerRoutine(DWORD error,
                            DWORD transferNumber, 
                            LPWSAOVERLAPPED overlapped, 
                            DWORD flags);
