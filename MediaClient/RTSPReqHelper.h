/**
 * @file RTSPReqHelper.h
 *
 * For Build RTSP requests, send request over TCP
 * and parse response simply.
 *
 * @author Chen xiaotian
 * @version 0.1 19/12/2019
 *
 */

#pragma once
#include <string>
#include "MySocket.h"
using namespace std;

class RTSPReqHelper
{
private:
	string url;
	int seq;
	string sessionId;
	MySocket socket;
	char recvBuff[1024];
public:
	RTSPReqHelper(void);
	// Getter & setter
	string getUrl();
	int getSeq();
	string getSessionId();
	void setUrl(string url);
	void setSessionId(string id);
	void setSocket(MySocket s);
	// Request
	string options();
	string describe();
	string setup(int rtpPort);
	string play();
	string pause();
	string teardown();
	string getParameter();
	~RTSPReqHelper(void);
};