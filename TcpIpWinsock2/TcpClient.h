#pragma once
#include <winsock2.h>
#include <thread>

class TcpClient
{
public:
	TcpClient();
	virtual ~TcpClient();

	
	void Connect(std::string address, int port);
	int  Send(LPBYTE sendBuffer, int sendSize);
	void Disconnect();

private:
	bool Init();
	bool _Connect();
	static void OnRecvThread(TcpClient* pClient);
	void _OnRecvThread();
	void StopRecvThread();

private:
	std::string address;
	int port;
	SOCKET sock;
	std::thread* pThread;
	BYTE rcvBuffer[1024];

	bool IsEnd;
};

