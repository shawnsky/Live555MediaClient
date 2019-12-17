#include <winsock2.h>
#pragma comment(lib, "WS2_32")
class CInitSock
{
public:
	CInitSock(BYTE minorVer = 2, BYTE majorVer = 2)
	{
		WSADATA wsaData;
		WORD sockVer = MAKEWORD(minorVer, majorVer);
		if (::WSAStartup(sockVer, &wsaData) != 0)
		{
			exit(0);
		}
	}
	~CInitSock()
	{
		::WSACleanup();
	}
};