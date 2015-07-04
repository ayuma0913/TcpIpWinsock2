#include "stdafx.h"
#include "TcpClient.h"
#include <sstream>

TcpClient::TcpClient()
{
}


TcpClient::~TcpClient()
{
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
			return 1;
		}

		unsigned int **addrptr = (unsigned int **)host->h_addr_list;

		while (*addrptr != NULL) 
		{
			server.sin_addr.S_un.S_addr = *(*addrptr);

			// connect()が成功したらloopを抜けます
			if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) 
			{
				break;
			}

			addrptr++;
			// connectが失敗したら次のアドレスで試す
		}

		// connectが全て失敗した場合
		if (*addrptr == NULL) 
		{
			printf("connect : %d\n", WSAGetLastError());
			return false;
		}
	}
	else
	{
		// inet_addr()が成功したとき

		// connectが失敗したらエラーを表示して終了
		if (connect(sock, (struct sockaddr *)&server, sizeof(server)) != 0) 
		{
			printf("connect : %d\n", WSAGetLastError());
			return false;
		}
	}

	return true;
}

int TcpClient::Send(LPBYTE sendBuffer, int sendSize)
{
	return send(sock, sendBuffer, sendSize, 0);
}

void TcpClient::Recv()
{
	memset(buf, 0, sizeof(buf));
	int n = recv(sock, buf, sizeof(buf), 0);
	if (n < 0) {
		printf("recv : %d\n", WSAGetLastError());
		return 1;
	}

	printf("%d, %s\n", n, buf);
}

void TcpClient::Disconnect()
{
	closesocket(sock);

	WSACleanup();
}
