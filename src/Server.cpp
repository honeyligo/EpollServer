/****************************************************************************************************
Server.cpp

Purpose:
	Server.cpp

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#include "../Common/Exception.h"
#include "../Common/threadpool.h"
#include "../Cached/Cached.h"
#include "../Cached/CachedMgr.h"
#include "Server.h"

#define		MAX_DB_CONNECTION			        100
#define		DEFAULT_DB_CONNECTION                           50

// public
Server::Server(const int& tcpPort, const int& udpPort, const int& workerSize,
		const int& taskSize, const int& timeout, const int& interval) :
	Listener(tcpPort, udpPort), m_isStart(false), m_workerSize(workerSize),
			m_queueSize(taskSize), m_timeout(timeout), m_interval(interval),
			m_pool(NULL), m_epollFd(0), m_nfds(0)
{
	TRY_
		/*init cached server*/
		CachedMgr::getInstance()->AddServer("127.0.0.1", 11211);

	CATCH_(ServerErrorException, "Server failed");
}

// public
Server::~Server(void)
{
	CachedMgr::delInstance();
	//Logger::DelInstance();
}

// public
void Server::Start(void)
{
	TRACE("this:0x%p", this);

	TRY_
		if (m_isStart == true)
			return;

		m_isStart = true;

		m_pool = threadpool_create(m_workerSize, m_queueSize, 0);
		if (m_pool == NULL) {
			THROW_E(ServerErrorException, "Start failed: fail to create thread pool");
		}

		/*		if (m_heartBeat != NULL)
		 {
		 m_heartBeat->SetInterval(m_interval);
		 m_heartBeat->SetTimeout(m_timeout);
		 m_heartBeat->Start();
		 }*/

		Listener::Start();

		m_epollFd = epoll_create(MAX_EVENTS);
		if (m_epollFd == -1) {
			THROW_E(ServerErrorException, "Start failed: fail to create epoll");
		}

		AddEvent(m_epollFd, m_listenFd, &m_ev);
		AddEvent(m_epollFd, m_udpFd, &m_ev);
		Dispatcher();

	CATCH_(ServerErrorException, "Start failed");
}

// public
void Server::Stop(void)
{
	if (m_isStart == false)
		return;

	m_isStart = false;

	TRY_
		threadpool_destroy(m_pool, 0);
		//m_heartBeat->Stop();
		Listener::Stop();

	CATCH_(ServerErrorException, "Stop failed");
}

// protected
void Server::Dispatcher(void)
{
	TRY_
		while (true) {
			m_nfds = epoll_wait(m_epollFd, m_events, MAX_EVENTS, -1);
			if (m_nfds == -1) {
				if (errno == EINTR) {
					continue;
				}
				TRACE("epoll_wait:%s", strerror(errno));
				//THROW_E(ServerErrorException, "Dispacher failed:  epoll_wait error");
			}

			if (m_nfds == 0)
				continue;

			int fd = 0;
			for (int i = 0; i < m_nfds; ++i) {
				//TRACE("event detected");

				ConnectionInfo* connInfo = new ConnectionInfo;
				connInfo->evModify = m_ev;
				connInfo->epFd = m_epollFd;
				connInfo->listenFd = m_listenFd;
				connInfo->udpFd = m_udpFd;
				connInfo->server = this;
				connInfo->evInvoke = m_events[i];
				fd = m_events[i].data.fd;

				DeleteEvent(m_epollFd, fd, &m_events[i]);
				threadpool_add(m_pool, &Worker, connInfo, 0);
			}

		}CATCH_(ServerErrorException, "Dispatcher failed");
}

// public
bool Server::IsStart(void)
{
	return m_isStart;
}

// public
void Server::Worker(void* arg)
{
	ConnectionInfo* conn = (ConnectionInfo*) arg;
	if (conn == NULL)
		return;

	Server* srv = conn->server;

	int connFd, fd;
	int n;
	struct sockaddr_in remote;
	int addrlen;

	TRY_
		fd = conn->evInvoke.data.fd;

		if (fd == conn->listenFd) {
			TRACE("connect from fd:%d in thread:%u", fd, pthread_self());
			addrlen = sizeof(struct sockaddr);
			while ((connFd = accept(fd, (struct sockaddr *) &remote,
					(socklen_t *) &addrlen)) > 0) {
				TRACE("new fd:%d in thread:%u", connFd, pthread_self());
				srv->SetNonblocking(connFd);
				srv->AddEvent(conn->epFd, connFd, &conn->evModify);
				//srv->GetClientAddr(connFd, inet_ntoa(remote.sin_addr));

				addrlen = sizeof(struct sockaddr);
			}
			srv->AddEvent(conn->epFd, conn->listenFd, &conn->evInvoke);

			if (connFd == -1) {
				if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO
						&& errno != EINTR)
					TRACE("accept error:%s", strerror(errno));
			}
		} else if (fd == conn->udpFd) {
			int count;//= srv->RecvUDPData(fd);
			if (count != 0)
				srv->AddEvent(conn->epFd, fd, &conn->evInvoke);
		} else if (conn->evInvoke.events & EPOLLIN) {
			int count;//= srv->RecvData(fd);

			if (count != 0)
				srv->AddEvent(conn->epFd, fd, &conn->evInvoke);
		} else if (conn->evInvoke.events & EPOLLOUT) {
			while (n > 0) {
				TRACE("send to fd:%d in thread:%u", fd, pthread_self());
				int nwrite = write(fd, "Hello", 6);
				if (nwrite < n) {
					if (nwrite == -1 && errno != EAGAIN) {
						TRACE("write error");
					}
					break;
				}
				n -= nwrite;
			}

			srv->AddEvent(conn->epFd, fd, &conn->evInvoke);
		}

	CATCH_(ServerErrorException, "Worker failed");

	delete conn;
}

// public
bool Server::AddEvent(int epfd, int fd, epoll_event* event)
{
	if (event == NULL)
		return false;

	event->events = EPOLLIN; //EPOLLONESHOT | EPOLLIN | EPOLLET;
	event->data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, event) == -1) {
		TRACE("epoll_ctl: add failed, fd:%d, error:%s", fd, strerror(errno));
		return false;
	}

	return true;
}

// public
bool Server::ModifyEvent(int epfd, int fd, epoll_event* event,
		epoll_event eventNodified)
{
	if (event == NULL)
		return false;

	event->events = eventNodified.events | EPOLLOUT;
	event->data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, event) == -1) {
		TRACE("epoll_ctl: modify failed, fd:%d, error:%s", fd, strerror(errno));
		return false;
	}

	return true;
}

// public
bool Server::DeleteEvent(int epfd, int fd, epoll_event* event)
{
	if (event == NULL)
		return false;

	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, event) == -1) {
		TRACE("epoll_ctl: delete failed, fd:%d, error:%s", fd, strerror(errno));
	}

	return true;
}
