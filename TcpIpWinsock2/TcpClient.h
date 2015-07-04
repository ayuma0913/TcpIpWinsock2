#pragma once
#include <winsock2.h>

class TcpClient
{
public:
	TcpClient();
	virtual ~TcpClient();

	bool Init();
	bool Connect(std::string address, int port);
	int  Send(LPBYTE sendBuffer, int sendSize);
	void Recv();
	void Disconnect();

private:
	SOCKET sock;
};

