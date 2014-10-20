/****************************************************************************************************
MsgHandler.h

Purpose:
	Message handler which implement the callbacks of IO message

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#ifndef __MSGHANDLER_H__
#define __MSGHANDLER_H__

#if PRAGMA_ONCE
#pragma once
#endif

#include <tr1/memory>
#include "../Common/Common.h"

#define MAX_EVENTS 1024*1024

class HeartBeatController;

class MsgHandler {
public:
	MsgHandler();
	virtual ~MsgHandler();

	void ProcessUserLogin(const Param& param);

	int RecvData(int fd);

	int RecvUDPData(int fd);

private:

	void ProcessData(const Param& param);

	/**
	 * 发送消息
	 *
	 * @param retCode: 成功报文为0， 失败报文为1
	 */
	void SendMessage(int fd, const string& json, int tranCode, int retCode,
			int version = 0, int checkCode = 0);

	void SendUDPMessage(const Param& param);

	void SendVPNTCPMessage(const Param& param, int defaultId);

	void ClearSessions(int fd);

	void CheckVPNReply(const string& relation);

	void GetClientAddr(int fd, char* addr);

private:

	int m_workerSize;
	int m_queueSize;
	int m_timeout;
	int m_interval;

	ThreadMutexLock m_lock;
	FdDataNode* m_node;
	std::tr1::shared_ptr<HeartBeatController> m_heartBeat;
};

#endif /* __MSGHANDLER_H__ */
