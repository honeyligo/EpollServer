/****************************************************************************************************
Server.h

Purpose:
	Server main class

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#ifndef __SERVER_H__
#define __SERVER_H__

#if PRAGMA_ONCE
#pragma once
#endif

#include <tr1/memory>
#include "../Common/Common.h"
#include "Listener.h"
#include "MsgHandler.h"

struct threadpool_t;
class HeartBeatController;


#define MAX_EVENTS 1024*1024

class Server: public Listener {
public:

	Server(const int& tcpPort, const int& udpPort, const int& workerSize,
			const int& taskSize, const int& timeout, const int& interval);
	virtual
	~Server(void);

	/**
	 * 开启服务方法
	 *
	 * @throw Exception 1.创建线程池失败; 2.创建epoll失败
	 */
	virtual void
	Start(void);

	/**
	 * stop server
	 */
	virtual void
	Stop(void);

	/**
	 * 服务状态获取方法
	 *
	 * @return bool 已开启返回true， 未开启返回false
	 */
	virtual bool
	IsStart(void);

private:
	/**
	 * 注册事件监听方法
	 *
	 * @return bool : 成功返回 true, 失败返回false
	 */
	bool
	AddEvent(int epfd, int fd, epoll_event* event);

	/**
	 * 修改事件监听方法
	 *
	 * @return bool : 成功返回true, 失败返回false
	 */
	bool
	ModifyEvent(int epfd, int fd, epoll_event* event,
					epoll_event eventNodified);

	/**
	 * 注销事件监听
	 *
	 * @return bool : 成功返回 true, 失败返回 false.
	 */
	bool
	DeleteEvent(int epfd, int fd, epoll_event* event);

	/**
	 * 线程工作方法
	 */
	static void
	Worker(void* arg);

	/**
	 * 事件分发方法
	 *
	 */
	void
	Dispatcher(void);

public:

private:
	bool m_isStart;
	int m_workerSize;
	int m_queueSize;
	int m_timeout;
	int m_interval;

	threadpool_t* m_pool;
	int m_epollFd;
	int m_nfds;
	struct epoll_event m_ev;
	struct epoll_event m_events[MAX_EVENTS];
	std::tr1::shared_ptr<HeartBeatController> m_heartBeat;
}; // End class Server

#endif // __SERVER_H__
