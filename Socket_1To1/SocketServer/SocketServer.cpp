
#include <iostream>
#include <cstring>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

using int32 = __int32;

using namespace std;

int main()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);	// 유동적으로 연결
	serverAddr.sin_port = ::htons(65456);

	::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	::listen(listenSocket, 10);

	bool flag = true;

	while (flag)
	{
		SOCKADDR_IN clientAddr;
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 addLen = sizeof(clientAddr);

		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addLen);

		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
		cout << "> echo-server is activated" << endl;
		cout << "> client connected by IP address " << ipAddress << " with Port number " << ::ntohs(serverAddr.sin_port) << endl;

		// TODO
		while (flag)
		{
			char recvBuffer[1000];

			int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

			if (!strncmp(recvBuffer, "quit", sizeof("quit")))
				flag = false;

			cout << "> echoed: " << recvBuffer << endl;

			int32 resultCode = ::send(clientSocket, recvBuffer, recvLen, 0);
		}
	}

	cout << "> echo-server is de-activated" << endl;

	::closesocket(listenSocket);

	::WSACleanup();

	return 0;
}