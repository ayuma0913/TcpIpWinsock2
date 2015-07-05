#pragma once
#include <winsock2.h>
#include <thread>

typedef void(*ONRECVFUNC)(LPBYTE,int);

class TcpClient
{
public:
	TcpClient();
	virtual ~TcpClient();

	
	void Connect(std::string address, int port, ONRECVFUNC onRecvFunc);
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
	ONRECVFUNC pRcvFunc;

	bool IsEnd;
};

