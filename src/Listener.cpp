/****************************************************************************************************
Listener.cpp

Purpose:
	provide initialization of server listening fd

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#include "Listener.h"

// public
Listener::Listener(const int& port, const int& udpPort)
	: m_isBindPort(false)
	, m_port(port)
	, m_udpPort(udpPort)
	, m_listenFd(0)
	, m_udpFd(0)
{

}

// public
Listener::~Listener(void)
{

}

// public
void Listener::Start(void)
{
	if (m_port <= 0)
		return;

	if ((m_listenFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		TRACE("sockfd");

	}

	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(m_port);

	SetNonblocking(m_listenFd);

	int flag = 1;
	int len = sizeof(int);
	if (setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEADDR, &flag, len) == -1) {
		TRACE("setsockopt reuse addr failed");
	}

	while (bind(m_listenFd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		TRACE("bind");
		sleep(5);

	}
	int lis = listen(m_listenFd, 20);
	TRACE("listen return:%d",lis);

	if (m_udpPort <= 0)
		return;

	if ((m_udpFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		TRACE("udp sockfd");

	}

	sockaddr_in sin_udp;
	memset(&sin_udp, 0, sizeof(sin_udp));
	sin_udp.sin_family = AF_INET;
	sin_udp.sin_port = htons(m_udpPort);

	SetNonblocking(m_udpFd);
	if (bind(m_udpFd, (struct sockaddr *) &sin_udp, sizeof(sin_udp)) < 0) {
		TRACE("udp bind");

	}

	TRACE("udp bind sucessfully");

	m_isBindPort = true;
}

// public
void Listener::Stop(void)
{
	m_isBindPort = false;
}

// public
bool Listener::IsBindPort(void)
{
	return m_isBindPort;
}

// public
void Listener::SetPort(const int& port)
{
	m_port = port;
}

// public
int Listener::GetPort(void)
{
	return m_port;
}

//// public
//void 
//Listener::OnConnectEvent(const int& sock)
//{
//
//}

// private
void Listener::SetNonblocking(int sockfd)
{
	int opts;

	opts = fcntl(sockfd, F_GETFL);
	if (opts < 0) {
		TRACE("fcntl(F_GETFL)");

	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(sockfd, F_SETFL, opts) < 0) {
		TRACE("fcntl(F_SETFL)");

	}
}
