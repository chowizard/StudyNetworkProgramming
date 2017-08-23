#include "stdafx.h"

#include <WinSock2.h>
#include <stdio.h>
#include <locale.h>

#define MAX_PACKET_LENGTH (1024)

static const int PORT = 3500;
static const char IP[] = "127.0.0.1";

//------------------------------------------------------------------------------
//int _tmain(int argc, _TCHAR* argv[])
int __CRTDECL wmain(int /*argc*/, wchar_t* /*argv*/[])
{
	setlocale(LC_ALL, "");

	WSADATA WindowsSocketAgent;
	int SocketAgent = WSAStartup(WINSOCK_VERSION, &WindowsSocketAgent);
	if(0 != SocketAgent)
	{
		return SEVERITY_ERROR;
	}

	// 소켓 생성
	SOCKET Socket = socket(AF_INET, 
						   SOCK_STREAM, 
						   IPPROTO_TCP);
	if(Socket == INVALID_SOCKET)
	{
		return SEVERITY_ERROR;
	}

	// 연결
	SOCKADDR_IN Address;
	Address.sin_family = AF_INET;
	Address.sin_port = htons(PORT);
	Address.sin_addr.S_un.S_addr = inet_addr(IP);
	if(SOCKET_ERROR == connect(Socket, 
							   reinterpret_cast<const sockaddr*>(&Address), 
							   sizeof(Address)))
	{
		wprintf_s(L"Fail to connect!\n");
		closesocket(Socket);

		return SEVERITY_ERROR;
	}

	wprintf_s(L"Enter message\n");

	char PacketBuffer[MAX_PACKET_LENGTH] = { 0 };
	int PacketIndex = 0;
	int PacketLength = 0;
	
	wchar_t InputBuffer[(MAX_PACKET_LENGTH / 2) + 1] = { 0 };
	int InputBufferIndex = 0;
	while(true)
	{
		wchar_t CurrentBuffer = getwchar();
		InputBuffer[InputBufferIndex++] = CurrentBuffer;

		if(CurrentBuffer == '\n')
		{
			InputBuffer[InputBufferIndex++] = '\0';
			break;
		}
	}

	WideCharToMultiByte(CP_ACP, 
						0, 
						InputBuffer, 
						wcslen(InputBuffer), 
						PacketBuffer, 
						MAX_PACKET_LENGTH, 
						NULL, 
						NULL);
	
	PacketLength = strlen(PacketBuffer) + 1;

	// 전송(Linux의 write())
	wchar_t MessageString[(MAX_PACKET_LENGTH / 2) + 1] = { 0 };
	MultiByteToWideChar(CP_ACP, 
						0, 
						PacketBuffer, 
						strlen(PacketBuffer), 
						MessageString, 
						(MAX_PACKET_LENGTH / 2) + 1);
	wprintf_s(L"Send Messages (%d Bytes) : %s\n", 
			  PacketLength, 
			  MessageString);
	int SendResult = send(Socket, 
						  PacketBuffer, 
						  PacketLength, 
						  0);

	// 수신(Linux의 read())
	int ReadBytes = 0;
	for(ReadBytes = 0; ReadBytes < PacketLength; )
	{
		ReadBytes += recv(Socket, 
						  PacketBuffer + ReadBytes, 
						  PacketLength - ReadBytes, 
						  0);
	}

	MultiByteToWideChar(CP_ACP, 
						0, 
						PacketBuffer, 
						strlen(PacketBuffer), 
						MessageString, 
						(MAX_PACKET_LENGTH / 2) + 1);
	wprintf_s(L"Recieve Messages = %s\n", MessageString);

	// 소켓 닫기
	closesocket(Socket);

	WSACleanup();

	return SEVERITY_SUCCESS;
}

