
#include <iostream>

#include <chrono>
#include <thread>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

using int32 = __int32;

using namespace std;

int main()
{
	// 윈도우 소켓 초기화 (ws2_32 라이브러리 초기화)
	// 관련 정보가 wsaData에 채워짐
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
		return 0;

	// ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
	// type : TCP(SOCK_STREAM), UDP(SOCK_DGRAM)
	// protocol : 0
	// return : descriptor
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN serverAddr;	// IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	//addr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	// host to network short
	// Big-Endian 으로 변환
	serverAddr.sin_port = ::htons(65456);


	::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	// 연결 성공

	cout << "> echo-client is activated" << endl;

	bool flag = true;

	while (flag)
	{
		char sendBuffer[100];
		cout << "> ";
		cin >> sendBuffer;

		if (!strncmp(sendBuffer, "quit", sizeof("quit")))
			flag = false;

		int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);

		char recvBuffer[1000];

		int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

		cout << "> received: " << recvBuffer << endl;

		this_thread::sleep_for(1s);
	}

	cout << "> echo-client is de-activated" << endl;

	// 소켓 리소스 반환
	::closesocket(clientSocket);

	// 윈도우 소켓 종료
	// WSAStartup과 1:1로 해주는 것이 권고사항
	::WSACleanup();

	return 0;
}
