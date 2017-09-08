#include "stdafx.h"

#include <WinSock2.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// 상수 정의
//------------------------------------------------------------------------------
static const int PACKET_LENGTH_MAX = 1024;
static const int PORT = 3500;

//------------------------------------------------------------------------------
int __CRTDECL wmain(int /*argc*/, wchar_t* /*argv*/[])
{
	WSADATA WindowsSocketAgent;

	char RecieveBuffer[PACKET_LENGTH_MAX] = { 0 };
	int ReadPosition = 0;
	int WritePosition = 0;
	int Length = 0;

	int SocketAgent = WSAStartup(WINSOCK_VERSION, &WindowsSocketAgent);
	if(0 != SocketAgent)
	{
		return SEVERITY_ERROR;
	}

	SOCKET ListenSocket = socket(AF_INET, 
								 SOCK_STREAM, 
								 IPPROTO_TCP);
	if(ListenSocket == INVALID_SOCKET)
	{
		return SEVERITY_ERROR;
	}

	struct sockaddr_in ServerAddress;
	ZeroMemory(&ServerAddress, sizeof(struct sockaddr_in));

	ServerAddress.sin_family = PF_INET;
	ServerAddress.sin_port = htons(PORT);
	ServerAddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if(SOCKET_ERROR == bind(ListenSocket, 
						    reinterpret_cast<const sockaddr*>(&ServerAddress), 
							sizeof(sockaddr_in)))
	{
		return SEVERITY_SUCCESS;
	}

	if(SOCKET_ERROR == listen(ListenSocket, 5))
	{
		return SEVERITY_SUCCESS;
	}

	SOCKET ClientSocket = INVALID_SOCKET;
	struct sockaddr_in ClientAddress;
	while(true)
	{
		ZeroMemory(&ClientAddress, sizeof(sockaddr_in));
		Length = sizeof(sockaddr_in);
		ClientSocket = accept(ListenSocket, 
							  reinterpret_cast<sockaddr*>(&ClientAddress), 
							  &Length);
		if(ClientSocket != INVALID_SOCKET)
		{
			char* ConnectedClientAddress = inet_ntoa(ClientAddress.sin_addr);
			wchar_t MessageString[(PACKET_LENGTH_MAX / 2) + 1] = { 0 };
			MultiByteToWideChar(CP_ACP, 
								0, 
								ConnectedClientAddress, 
								strlen(ConnectedClientAddress), 
								MessageString, 
								(PACKET_LENGTH_MAX / 2) + 1);

			wprintf_s(L"New Client is connected : %s\n", 
					  MessageString);
		}

		// 읽기(Linux의 read())
		ReadPosition = recv(ClientSocket, 
							RecieveBuffer, 
							PACKET_LENGTH_MAX, 
							0);

		if(ReadPosition > 0)
		{
			WritePosition = send(ClientSocket, 
								 RecieveBuffer, 
								 ReadPosition, 
								 0);			
		}

		closesocket(ClientSocket);
	}

	// 소켓 닫기
	closesocket(ListenSocket);

	WSACleanup();

	return SEVERITY_SUCCESS;
}