#include "stdafx.h"
#include "RTSPReqHelper.h"

RTSPReqHelper::RTSPReqHelper(void)
{
	seq = 2;
}

string RTSPReqHelper::getUrl()
{
	return url;
}
int RTSPReqHelper::getSeq()
{
	return seq;
}
string RTSPReqHelper::getSessionId()
{
	return sessionId;
}
void RTSPReqHelper::setUrl(string str)
{
	url = str;
}
void RTSPReqHelper::setSessionId(string str)
{
	sessionId = str;
}
void RTSPReqHelper::setSocket(MySocket s)
{
	socket = s;
}
string parseValueOf(string src, string field)
{
	int beg = src.find(field);
	int end = src.find("\r\n", beg);
	return src.substr(beg + field.size(), end - beg - field.size());
}
string RTSPReqHelper::options()
{
	// Build Request Line
	string line = "OPTIONS ";
	line.append(url);
	line.append(" RTSP/1.0\r\n");
	line.append("CSeq: ");
	line.append(to_string(seq++));
	line.append("\r\n");
	line.append("User-Agent: Cxt Media Client v0.1\r\n\r\n");
	// Send over TCP
	socket.Send(line);
	// Handle Response
	memset(recvBuff, 0, sizeof(recvBuff));
	socket.TCPRecv(recvBuff, sizeof(recvBuff));
	string s(recvBuff);
	string cseq = parseValueOf(s, "CSeq: ");
	string status = parseValueOf(s, " ");
	string options = parseValueOf(s, "Public: ");
	if (to_string(seq-1).compare(cseq) != 0 || status.compare("200 OK") != 0)
	{
		return "ERROR";
	}
	return options;
}
string RTSPReqHelper::describe()
{
	// Build Request Line
	string line = "DESCRIBE ";
	line.append(url);
	line.append(" RTSP/1.0\r\n");
	line.append("CSeq: ");
	line.append(to_string(seq++));
	line.append("\r\n");
	line.append("User-Agent: Cxt Media Client v0.1\r\n");
	line.append("Accept: application/sdp\r\n\r\n");
	// Send over TCP
	socket.Send(line);
	// Handle Response
	memset(recvBuff, 0, sizeof(recvBuff));
	socket.TCPRecv(recvBuff, sizeof(recvBuff));
	string s(recvBuff);
	string cseq = parseValueOf(s, "CSeq: ");
	string status = parseValueOf(s, " ");
	if (to_string(seq - 1).compare(cseq) != 0 || status.compare("200 OK") != 0)
	{
		return "ERROR";
	}
	//string newUrl = parseValueOf(s, "Content-Base: ");
	//setUrl(newUrl);

	return s;
}
string RTSPReqHelper::setup(int rtpPort)
{
	// Build Request Line
	string line = "SETUP ";
	line.append(url);
	line.append(" RTSP/1.0\r\n");
	line.append("CSeq: ");
	line.append(to_string(seq++));
	line.append("\r\n");
	line.append("User-Agent: Cxt Media Client v0.1\r\n");
	line.append("Transport: RTP/AVP;unicast;client_port=");
	line.append(to_string(rtpPort));
	line.append("-");
	line.append(to_string(rtpPort + 1));
	line.append("\r\n\r\n");
	// Send over TCP
	socket.Send(line);
	// Handle Response
	memset(recvBuff, 0, sizeof(recvBuff));
	socket.TCPRecv(recvBuff, sizeof(recvBuff));
	string s(recvBuff);
	string cseq = parseValueOf(s, "CSeq: ");
	string status = parseValueOf(s, " ");
	if (to_string(seq - 1).compare(cseq) != 0 || status.compare("200 OK") != 0)
	{
		return "ERROR";
	}
	string sid = parseValueOf(s, "Session: ");
	//  extra check
	int I1l = sid.find(";timeout");
	if (I1l)
	{
		sid = sid.substr(0, I1l);
	}
	setSessionId(sid);
	return sid;
}
string RTSPReqHelper::play()
{
	// Build Request Line
	string line = "PLAY ";
	line.append(url);
	line.append(" RTSP/1.0\r\n");
	line.append("CSeq: ");
	line.append(to_string(seq++));
	line.append("\r\n");
	line.append("User-Agent: Cxt Media Client v0.1\r\n");
	line.append("Session: ");
	line.append(sessionId);
	line.append("\r\n");
	//line.append("Range: smpte=0:00:00-\r\n\r\n");
	line.append("Range: npt=0.000-\r\n\r\n");
	// Send over TCP
	socket.Send(line);
	// Handle Response
	memset(recvBuff, 0, sizeof(recvBuff));
	socket.TCPRecv(recvBuff, sizeof(recvBuff));
	string s(recvBuff);
	string cseq = parseValueOf(s, "CSeq: ");
	string status = parseValueOf(s, " ");
	if (to_string(seq - 1).compare(cseq) != 0 || status.compare("200 OK") != 0)
	{
		return "ERROR";
	}
	return s;
}
string RTSPReqHelper::pause()
{
	string line = "PAUSE ";
	line.append(url);
	line.append(" RTSP/1.0\r\n");
	line.append("CSeq: ");
	line.append(to_string(++seq));
	line.append("\r\n");
	line.append("User-Agent: Cxt Media Client v0.1\r\n");
	line.append("Session: ");
	line.append(sessionId);
	line.append("\r\n\r\n");
	return line;
}
string RTSPReqHelper::teardown()
{
	string line = "TEARDOWN ";
	line.append(url);
	line.append(" RTSP/1.0\r\n");
	line.append("CSeq: ");
	line.append(to_string(++seq));
	line.append("\r\n");
	line.append("User-Agent: Cxt Media Client v0.1\r\n");
	line.append("Session: ");
	line.append(sessionId);
	line.append("\r\n\r\n");
	return line;
}
string RTSPReqHelper::getParameter()
{
	string line = "GET_PARAMETER ";
	line.append(url);
	line.append(" RTSP/1.0\r\n");
	line.append("CSeq: ");
	line.append(to_string(++seq));
	line.append("\r\n");
	line.append("User-Agent: Cxt Media Client v0.1\r\n");
	line.append("Session: ");
	line.append(sessionId);
	line.append("\r\n\r\n");
	return line;
}
RTSPReqHelper::~RTSPReqHelper(void)
{
}
