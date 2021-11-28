
#include <iostream>

#include <chrono>
#include <thread>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

using int32 = __int32;
using namespace std;

bool flag = true;

void HandleError(const char* cause);
void SendData(SOCKET clientSocket, SOCKADDR_IN serverAddr);
void ReceiveData(SOCKET clientSocket);

int main()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
		return 0;

	// UDP : SOCK_DGRAM
	SOCKET clientSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(65456);

	// Connect
	::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	cout << "> echo-client is activated" << endl;

	// Send
	std::thread(SendData, clientSocket, serverAddr).detach();
	// Receive
	std::thread(ReceiveData, clientSocket).detach();

	while (flag)
	{
	}
	
	cout << "> echo-client is de-activated" << endl;

	::closesocket(clientSocket);

	::WSACleanup();

	return 0;
}

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

void SendData(SOCKET clientSocket, SOCKADDR_IN serverAddr)
{
	// Send
	char sendBuffer[1024];
	while (flag)
	{
		cout << "> "; cin >> sendBuffer;

		int32 resultCode = ::sendto(clientSocket, sendBuffer, sizeof(sendBuffer), 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

		if (resultCode == SOCKET_ERROR)
		{
			HandleError("SendTo");
			return;
		}

		if (!strncmp(sendBuffer, "quit", sizeof("quit")))
			flag = false;
	}
}

void ReceiveData(SOCKET clientSocket)
{
	SOCKADDR_IN recvAddr;
	::memset(&recvAddr, 0, sizeof(recvAddr));
	int32 addrLen = sizeof(recvAddr);

	char recvBuffer[1024];
	while (flag)
	{
		int32 recvLen = ::recvfrom(clientSocket, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&recvAddr, &addrLen);
		if (recvLen <= 0)
		{
			HandleError("RecvFrom");
			return;
		}
		cout << "> received: " << recvBuffer << endl;
	}
}