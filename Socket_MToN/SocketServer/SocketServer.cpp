
#include <iostream>
#include <cstring>
#include <vector>

#include "Socket.h"

void HandleError(const char* cause);
void ReceiveData(SOCKET clientSocket, int number);
void AcceptClients(SOCKET listenSocket, SOCKADDR_IN* serverAddr);

vector<Socket> socketVector;
char recvBuffer[1024];
bool flag = true;

int main()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}

	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
	{
		HandleError("Non Blocking");
		return 0;
	}

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(65456);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		HandleError("Bind");
		return 0;
	}

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		HandleError("Listen");
		return 0;
	}

	cout << "> echo-server is activated" << endl;
	cout << "> server loop running in thread (main thread): Thread-1" << endl;

	thread(AcceptClients, listenSocket, &serverAddr).detach();

	char quitBuffer[15];
	while (flag)
	{
		cin >> quitBuffer;
		if (!strncmp(quitBuffer, "quit", sizeof("quit")))
		{
			if (Socket::GetClientNumber() == 1)
				flag = false;
			else
				cout << "> active threads are remained : " << Socket::GetClientNumber() - 1 << " threads" << endl;
		}
	}

	cout << "> echo-server is de-activated" << endl;

	::closesocket(listenSocket);

	::WSACleanup();

	return 0;
}

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

void ReceiveData(SOCKET clientSocket, int number)
{
	// Receive
	while (flag)
	{
		int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

		if (recvLen == SOCKET_ERROR)
		{
			// 논블로킹이면
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			// Error
			HandleError("Receive");
			break;
		}
		else if (recvLen == 0)
		{
			// 연결 끊김
			HandleError("Receive");
			break;
		}

		if (!strncmp(recvBuffer, "quit", sizeof("quit")))
		{
			cout << "> echoed: quit by Thread-" << number + 2 << endl;
			Socket::SubClientNumber();
			continue;
		}

		// Send
		while (flag)
		{
			cout << "> received ( " << recvBuffer << " ) and echoed to " << Socket::GetClientNumber() - 1 << " threads" << endl;
			for (int i = 0; i < Socket::GetClientNumber() - 1; ++i)
			{
				if (::send(socketVector[i].GetClientSocket(), recvBuffer, sizeof(recvBuffer), 0) == SOCKET_ERROR)
				{
					// 논블로킹이면
					if (::WSAGetLastError() == WSAEWOULDBLOCK)
						continue;
					
					// Error
					HandleError("Send");
					break;
				}
			}
			break;
		}
	}
	return;
}

void AcceptClients(SOCKET listenSocket, SOCKADDR_IN* serverAddr)
{
	int number = 0;

	while (flag)
	{
		socketVector.push_back(Socket());
		socketVector[number].SetClientSocket(::accept(listenSocket, (SOCKADDR*)&socketVector[number].GetClientAddr(), &socketVector[number].GetClientSize()));

		if (socketVector[number].GetClientSocket() == INVALID_SOCKET)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			HandleError("Accept");
			return;
		}

		Socket::AddClientNumber();
		socketVector[number].SetNumber(number);

		char ipAddress[16];
		::inet_ntop(AF_INET, &socketVector[number].GetClientAddr().sin_addr, ipAddress, sizeof(ipAddress));
		cout << "> client connected by IP address " << ipAddress << " with Port number " << ::ntohs((*serverAddr).sin_port) << endl;
		
		std::thread(ReceiveData, socketVector[number].GetClientSocket(), socketVector[number].GetNumber()).detach();

		number++;
	}

	return;
}
