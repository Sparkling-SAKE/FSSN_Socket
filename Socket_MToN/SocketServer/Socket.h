
#pragma once

#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

using int32 = __int32;
using namespace std;

class Socket
{
private:
	SOCKET			clientSocket;
	SOCKADDR_IN		clientAddr;
	int				clientSize;
	int				number;
private:
	static atomic<int32> clientNumber;

public:
	Socket()
	{
		::memset(&clientSocket, 0, sizeof(clientSocket));
		::memset(&clientAddr, 0, sizeof(clientAddr));
		clientSize = sizeof(clientAddr);
		number = 0;
	}
	~Socket()
	{
		::memset(&clientSocket, 0, sizeof(clientSocket));
		::memset(&clientAddr, 0, sizeof(clientAddr));
		clientSize = -1;
		number = -1;
	}

public:
	void SetClientSocket(SOCKET socket) { clientSocket = socket; }
	void SetNumber(int num) { number = num; }

	SOCKADDR_IN& GetClientAddr()	{ return clientAddr; }
	SOCKET& GetClientSocket()		{ return clientSocket; }
	int& GetClientSize()			{ return clientSize; }
	int& GetNumber()				{ return number; }

public:
	static int32	GetClientNumber() { return clientNumber; }
	static void		AddClientNumber() { clientNumber.fetch_add(1); }
	static void		SubClientNumber() { clientNumber.fetch_add(-1); }

};

