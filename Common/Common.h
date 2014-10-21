/****************************************************************************************************
Common.h

Purpose:
	provide common header files and public methods

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#ifndef __COMMON_H__
#define __COMMON_H__

#if PRAGMA_ONCE
#pragma once
#endif

#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdarg.h>
#include <errno.h>
#include <memory.h>
#include <time.h>

#include <string>
#include <sstream>
#include <fstream>

#include <curl/curl.h>

#include <json/json.h>
#include "tinyxml.h"
#include "tinystr.h"

#include "Lock.h"
#include "../Common/Exception.h"

//#define DEBUG


#define LOG_PATH	"./log"

using namespace std;

template<typename T>
string NumberToString(T Number)
{
	ostringstream ss;
	ss << Number;
	return ss.str();
}

template<typename T>
T StringToNumber(const string &Text)
{
	istringstream ss(Text);
	T result;
	return ss >> result ? result : 0;
}

class RandomStringGenerator {
public:
	typedef enum {
		NUMBER, CHARACTERS, MIX
	} RANDOM_TYPE;

public:
	static string generate(std::size_t len = 1, RANDOM_TYPE type = MIX)
	{
		string str;
		switch (type) {
		case NUMBER:
			str = "0123456789";
			break;
		case CHARACTERS:
			str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
			break;
		case MIX:
			str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
			break;
		default:
			break;
		}

		std::string seq;
		std::size_t size = str.size();
		if (size)
			while (len--)
				seq.push_back(str[rand() % size]);
		return seq;
	}

private:
	RandomStringGenerator(void)
	{
	}

private:
	//string str_;
};

class Server;

typedef struct ConnectionInfo {
	ConnectionInfo(void) :
		epFd(0), listenFd(0), udpFd(0), evModify(), evInvoke(), server(NULL)
	{

	}
	int epFd; // epoll���
	int listenFd; // ������
	int udpFd;
	struct epoll_event evModify; // �޸ĵ��¼�
	struct epoll_event evInvoke; // ����������
	Server* server;
} ConnInfo;


typedef struct MessageHeader {
	MessageHeader(void) :
		body_len(0), tran_code(0), version(0), check_code(0), request_id(0),
				retcode(0)
	{
		memset(request_sign, 0, 32);
		memset(trace_no, 0, 16);
		memset(sign, 0, 32);
		memset(reserve, 0, 16);
	}

	int body_len;
	int tran_code;
	int version;
	int check_code;
	int request_id;
	char request_sign[32];
	char trace_no[16];
	char sign[32];
	int retcode;
	char reserve[16];
} MessageHeader;

typedef struct Param {
	Param(void) :
		msg_type(0), data_len(0), fd(0), port(0), tran_code(0), pData(NULL),
				ip(NULL), wan_addr()
	{
		memset(login_key, 0, 32);
		memset(pair_key, 0, 32);
		memset(robot_sn, 0, 32);
		memset(phone_no, 0, 32);
	}
	int msg_type;
	int data_len;
	int fd;
	int port;
	int tran_code;

	char* pData;
	char* ip;

	struct sockaddr_in wan_addr;

	char login_key[32];
	char pair_key[32];
	char robot_sn[32];
	char phone_no[32];
} Param;

typedef struct FdDataNode {
	FdDataNode() :
		header(), lan_addr(), recv_len(0), lan_port(0), is_login(0)
	//, lock ()
	{
		memset(buf, 0, 2048);
		memset(wan_addr, 0, 32);
		memset(lan_ip, 0, 32);
		memset(login_key, 0, 32);
		memset(pair_key, 0, 32);
	}
	~FdDataNode()
	{

	}
	MessageHeader header;
	struct sockaddr_in lan_addr;
	int recv_len;
	int lan_port;
	bool is_login;
	char buf[2048];
	char wan_addr[32];
	char lan_ip[32];
	char login_key[32];
	char pair_key[32];

} FdDataNode;

typedef struct Session {
	Session(void) :
		fd(0), power(0), time(), wan_port(0), udp_received(false)
	{
		memset(wan_addr, 0, 32);
		memset(robot_sn, 0, 32);
		memset(phone_no, 0, 32);
	}
	int fd;
	int power;
	time_t time;
	//bool status;
	//ThreadMutexLock lock;
	char wan_addr[32];
	int wan_port;

	char robot_sn[32];
	char phone_no[32];

	bool udp_received;
} Session;

typedef struct Reply {
	Reply(void) :
		done(false)
	{

	}
	bool done;
	//ThreadMutexLock lock;
} Reply;

class Logger {
public:
	static Logger *GetInstance(void);
	void Write(const char* fileName, int line, const char* func,
			const char *format, ...);

private:
	std::string DateFormat();
	static void DelInstance(void);

private:
	static ThreadMutexLock* lock;
	static Logger* instance;
	static ofstream ofstr;
};

class CurlTool {
public:
	typedef struct response {
		response(void) :
			code(0), body()
		{

		}
		int code;
		std::string body;
	} response;

	typedef struct request {
		request(void) :
			data(NULL), len(0)
		{

		}
		const char* data;
		size_t len;
	} request;

	static response Post(const std::string& url, const std::string& data,
			const std::string& auth);

	static size_t WriteCallback(void *data, size_t size, size_t nmemb,
			void *userData);

};


/* macros */

#define FILE_BASENAME strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__
#define TRACE(fmt, ...) 	Logger::GetInstance()->Write(FILE_BASENAME, __LINE__, __FUNCTION__, fmt, ## __VA_ARGS__);

#define CHECK_AND_SET(str, rs, column)                                                                                       \
do{                                                                                                                                                         \
  char* tmp = NULL;                                                                                                                                            \
  if ((tmp = const_cast<char*>(rs->getString(column))) != NULL)                                                              \
      str = tmp;                                                                                                                                          \
} while(0)

#define CHECK_CONNECTION_STATUS(fd)											\
	do{																										\
		bool status = m_node[fd].is_login;					\
			if(!status)																\
			{																						\
				TRACE("close invalid connection fd:%d", fd);		\
				close(fd);																\
				ClearSessions(fd);											\
				return;																	\
			}																					\
}while(0)

#endif /* __COMMON_H__ */
