#include "stdafx.h"
#include "TcpClient.h"
#include <sstream>


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
	
}


bool TcpClient::Connect(std::string address, int port)
{
	this->address = address;
	this->port = port;

	
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
	server.sin_addr.S_un.S_addr = inet_addr(deststr);

	if (server.sin_addr.S_un.S_addr == 0xffffffff)
	{
		struct hostent *host;

		host = gethostbyname(deststr);
		if (host == NULL)
		{
			if (WSAGetLastError() == WSAHOST_NOT_FOUND)
			{
				printf("host not found : %s\n", deststr);
			}
			return false;
		}

		unsigned int **addrptr = (unsigned int **)host->h_addr_list;

		while (*addrptr != NULL)
		{
			server.sin_addr.S_un.S_addr = *(*addrptr);

			// connect()������������loop�𔲂��܂�
			if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0)
			{
				break;
			}

			addrptr++;
			// connect�����s�����玟�̃A�h���X�Ŏ���
		}

		// connect���S�Ď��s�����ꍇ
		if (*addrptr == NULL)
		{
			printf("connect : %d\n", WSAGetLastError());
			return false;
		}
	}
	else
	{
		// inet_addr()�����������Ƃ�

		// connect�����s������G���[��\�����ďI��
		if (connect(sock, (struct sockaddr *)&server, sizeof(server)) != 0)
		{
			printf("connect : %d\n", WSAGetLastError());
			return false;
		}
	}

}

void TcpClient::OnRecvThread(TcpClient* pClient)
{
	pClient->_OnRecvThread();
}
void TcpClient::_OnRecvThread()
{
	while (!IsEnd)
	{
		int rcvResutl = recv(sock, (char*)rcvBuffer, sizeof(rcvBuffer), 0);

		if (rcvResutl <= 0 && !IsEnd)
		{
			// �ڑ����؂ꂽ����Đڑ������݂�
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