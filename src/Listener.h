/****************************************************************************************************
Listener.h

Purpose:
	provide initialization of server listening fd

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#ifndef __LISTENER__H__
#define __LISTENER__H__

#include "../Common/Common.h"

using namespace std;

/*
 #if PRAGMA_ONCE
 #pragma once
 #endif
 */

class Listener {
public:
	/**
	 * ����/��������
	 *
	 * @param port ����󶨶˿�
	 *
	 */
	Listener(const int& port, const int& udpPort);
	virtual ~Listener(void);

	/***
	 * ��������
	 *
	 * @throw Exception 1.û�����ö˿�; 2.���ö˿ڷǷ�
	 */
	virtual void Start(void);

	/***
	 * ֹͣ����
	 */
	virtual void Stop(void);

	/**
	 * ���ö˿�
	 *
	 * @param port ����󶨶˿�
	 */
	void SetPort(const int& port);

	/**
	 * ��ȡ�˿�
	 *
	 * @return int ����˿�
	 */
	int GetPort(void);

	/**
	 * �Ƿ����˿�
	 *
	 * @return bool �Ƿ����˿�
	 */
	virtual bool IsBindPort(void);

protected:
	/**
	 * ���յ�һ�����ӻص�����
	 *
	 * @param ���յ������ӻص�����
	 */
	//virtual void OnConnectEvent(const int& sock) = 0;


	/***
	 * ����socket����Ϊ������ģʽ
	 *
	 */
	void SetNonblocking(int sockfd);

protected:
	bool m_isBindPort;
	int m_port;
	int m_udpPort;
	int m_listenFd;
	int m_udpFd;
}; // End class Listener

#endif // __LISTENER__H__
