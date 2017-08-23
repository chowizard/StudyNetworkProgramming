#pragma once

#include <Windows.h>

//#if BLOCKED
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
SocketInfo* socketInfos[WSA_MAXIMUM_WAIT_EVENTS];
WSAEVENT windowSocketEvents[WSA_MAXIMUM_WAIT_EVENTS];
int eventTotalCount = 0;
CRITICAL_SECTION criticalSection;

SOCKET CreateServerSocket(void);
SocketInfo* CreateSocketInfo(SOCKET socket);
void RecieveSocketEvent(SOCKET acceptedSocket);
void RemoveSocketEvent(int eventIndex);
void ReleaseSocketInfoAndEvent(WSAEVENT socketEvent, SocketInfo* socketInfo, int eventIndex);
DWORD WINAPI ThreadProcess(LPVOID argv);
//#endif