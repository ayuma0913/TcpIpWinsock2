#include "stdafx.h"
#include "TcpClient.h"
#include <sstream>
#include <ws2def.h>
#include <WS2tcpip.h> 

TcpClient::TcpClient()
	:pThread(NULL)
	, IsEnd(false)
{
	Init();
}


TcpClient::~TcpClient()
{
	WSACleanup();
}


bool TcpClient::Init()
{
	WSADATA wsaData;
	
	
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		OutputDebugString(L"WSAStartup failed\n");
		return false;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::stringstream error;
		error << "socket : " << WSAGetLastError();
		OutputDebugStringA(error.str().c_str());
		return false;
	}
	
	return true;
}


void TcpClient::Connect(std::string address, int port, ONRECVFUNC onRecvFunc)
{
	this->address = address;
	this->port = port;
	this->pRcvFunc = onRecvFunc;
	
	StopRecvThread();

	pThread = new std::thread(OnRecvThread, this);

}

int TcpClient::Send(LPBYTE sendBuffer, int sendSize)
{
	return send(sock, (const char*)sendBuffer, sendSize, 0);
}


void TcpClient::Disconnect()
{
	StopRecvThread();
	closesocket(sock);

	
}

bool TcpClient::_Connect()
{
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons((u_short)port);

	const char* deststr = address.c_str();

	if (inet_pton(AF_INET, deststr, &server.sin_addr.S_un) != 1)
	{
		struct addrinfo hints;
		struct addrinfo* address;
		::ZeroMemory(&hints, sizeof(hints));
		// ヒントとしてIPネットワークプロトコルとTCP通信のソケットタイプを指定
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;

		if (getaddrinfo(deststr, std::to_string(port).c_str(), &hints, &address))
		{
			if (WSAGetLastError() == WSAHOST_NOT_FOUND)
			{
				printf("host not found : %s\n", deststr);
			}
			return false;
		}

		if (connect(sock, address->ai_addr, address->ai_addrlen) != 0)
		{
			printf("connect : %d\n", WSAGetLastError());
			return false;
		}
	}
	else
	{
		if (connect(sock, (struct sockaddr *)&server, sizeof(server)) != 0)
		{
			printf("connect : %d\n", WSAGetLastError());
			return false;
		}
	}

	return true;
}

void TcpClient::OnRecvThread(TcpClient* pClient)
{
	pClient->_OnRecvThread();
}
void TcpClient::_OnRecvThread()
{
	while (!IsEnd)
	{
		int rcvResult = recv(sock, (char*)rcvBuffer, sizeof(rcvBuffer), 0);

		if (rcvResult > 0)
		{
			if (pRcvFunc) pRcvFunc(rcvBuffer, rcvResult);
		}
		else if (!IsEnd)
		{
			// 接続が切れたから再接続を試みる
			if (!_Connect())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}


	}
	
}

void TcpClient::StopRecvThread()
{
	if (pThread)
	{
		if (pThread->joinable())
		{
			pThread->join();
		}

		delete pThread;
	}
}