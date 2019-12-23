/**
 * @file MySocket.h
 *
 * Wrapper Windows socket operate
 *
 *
 * @author Chen xiaotian
 * @version 0.1 19/12/2019
 *
 */
#pragma once
#include <string>
#include "CInitSock.h"
class MySocket
{
private:
	CInitSock sock;
	SOCKET socket;
public:
	MySocket()
	{
		socket = INVALID_SOCKET;
	}
	bool Create(std::string protocol)
	{
		if (protocol == "TCP")
		{
			socket = ::socket(AF_INET, SOCK_STREAM, 0);
		}
		else
		{
			socket = ::socket(AF_INET, SOCK_DGRAM, 0);
		}
		if (socket == INVALID_SOCKET) {
			return false;
		}
		return true;
	}
	bool Bind(int port)
	{
		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);
		sin.sin_addr.s_addr = htonl(ADDR_ANY);
		if (::bind(socket, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
		{
			return false;
		}
		return true;
	}
	bool Connect(char *ip, int port)
	{
		sockaddr_in server_sin;
		int adddr_len = sizeof(server_sin);
		/* set media server address */
		memset((void*)&server_sin, 0, adddr_len);
		server_sin.sin_family = AF_INET;
		server_sin.sin_port = htons(port);
		server_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		/* do connect */
		if (::connect(socket, (sockaddr *)&server_sin, adddr_len) != 0)
		{
			return false;
		}
		return true;
	}
	int Send(std::string str)
	{
		int iResult;
		iResult = ::send(socket, str.c_str(), str.size(), 0);
		return iResult;
	}
	int TCPRecv(char *buff, int len)
	{
		int iResult;
		iResult = ::recv(socket, buff, len, 0);
		return iResult;
	}
	int UDPRecv(char *buff, int len)
	{
		int iResult;
		/* no need to save sockaddr */
		iResult = ::recvfrom(socket, buff, len, 0, NULL, NULL);
		return iResult;
	}
	void Close()
	{
		::closesocket(socket);
		socket = INVALID_SOCKET;
	}
	
};
