/****************************************************************************************************
MsgHandler.cpp

Purpose:
	MsgHandler.cpp implementation

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#include "../Common/threadpool.h"
#include "HeartBeatController.h"
#include "MsgHandler.h"

#define  	BUFFER_LEN 					2048
#define  	HEADER_LEN 					120
#define 	CLIENT 						(CachedMgr::getInstance()->GetHandler())
#define		FD_DATA_NODE_COUNT		                2048
using namespace ZDB;
//using namespace boost;

MsgHandler::MsgHandler()
{
	TRY_
	/* malloc fd buffer array */
		m_node = (FdDataNode*) malloc(sizeof(FdDataNode) * FD_DATA_NODE_COUNT);
		for (int i = 0; i < FD_DATA_NODE_COUNT; i++) {
			memset(m_node[i].buf, 0, BUFFER_LEN);
			m_node[i].recv_len = 0;
		}

		m_heartBeat = std::tr1::shared_ptr<HeartBeatController>(
				new HeartBeatController(this));
	CATCH_(ServerErrorException, "Server failed");
}

MsgHandler::~MsgHandler()
{
	if (m_node != NULL) {
		free(m_node);
	}
}

void MsgHandler::ProcessUserLogin(const Param& param)
{
	TRACE("Begin");
	char *pData = param.pData;
	int fd = param.fd;
	int data_len = param.data_len;

	Json::Reader reader;
	Json::Value value;

	TRY_
		string data(pData, data_len);
		TRACE("input data: %s", data.c_str());
		if (reader.parse(data, value))
		{
			string user_name = value["user_name"].asString();
			string password = value["passwd"].asString();

			string sql = "select id, flag_password from user where user_name = '"
				+ user_name + "' and password = '" + password + "'";

			TRACE("sql: %s", sql.c_str());

			AutoLock<ThreadMutexLock> lock(&m_lock);
			ZDB::ResultSet* rs;
			IDBOperator* p = NULL;
			{
				m_dbp->SetQuery(sql);
				rs = m_dbp->ExecQuery(p);
			}
			if (rs->next())
			{
				/* insert ONLINE user into map */
				{
					AutoLock<ThreadMutexLock> lock_(&m_lock);
					std::map<string, Session>::iterator it = m_user.find(user_name);
					if (it != m_user.end())
					{
						// clear power of robot
						std::map<string, Session>::iterator it_robot = m_robot.find(
							(*it).second.robot_sn);
						if (it_robot != m_robot.end())
						{
							(*it_robot).second.power = 0;
							(*it_robot).second.udp_received = false;
						}

						if (m_user[user_name].fd != fd)
						{
							// close the former fd
							close(m_user[user_name].fd);

							// clear all session of the former user
							TRACE("old fd:%d", m_user[user_name].fd);
							//delete (*it).second;
							m_user.erase(user_name);
						}
					}

					// update status
					m_node[fd].is_login = true;

					Session session;
					TRACE("session.udp_received:%d", session.udp_received);
					session.fd = fd;
					std::pair<std::map<string, Session>::iterator, bool> ret =
						m_user.insert(std::pair<string, Session>(user_name, session));
					if (ret.second)
					{
						TRACE("inserted fd:%d", m_user[user_name].fd);
					}
					else
					{
						TRACE("inserted failed");
					}
				}

				Json::Value response;
				response["login_key"] = RandomStringGenerator::generate(32,
					RandomStringGenerator::RANDOM_TYPE::MIX);

				std::string strValue = response.toStyledString();
				TRACE("user login response json: %s", strValue.c_str());

				SendMessage(fd, strValue, USER_LOGIN, 0);
			}
			else
			{
				TRACE("fail to parse json");
			}CATCH_(ServerErrorException, "ProcessUserLogin failed");
		}
	TRACE("End\n");
}

void MsgHandler::ProcessRobotHeartBeat(const Param& param)
{
TRACE("Begin");
	char *pData = param.pData;
	int fd = param.fd;
	int data_len = param.data_len;

	CHECK_CONNECTION_STATUS(fd);

	Json::Reader reader;
	Json::Value value;

	TRY_
		string data(pData, data_len);
		//TRACE("input data:%s", data.c_str());
		if (reader.parse(data, value))
		{
			string robot_sn = value["robot_sn"].asString();
			int key = value["key"].asInt();
			string soft_version = value["soft_version"].asString();

			string sql = "select id from robot where robot_sn = '" + robot_sn + "'";
			TRACE("robotFd:%d, robot_sn:%s, key:%d, soft_version: %s", fd,
					robot_sn.c_str(), key, soft_version.c_str());

			AutoLock<ThreadMutexLock> lock(&m_lock);
			ZDB::ResultSet* rs;
			IDBOperator* p = NULL;
			{
				m_dbp->SetQuery(sql);
				rs = m_dbp->ExecQuery(p);
			}
			if (rs->next())
			{
				//insert phone-fd, phone-sn
				std::map<string, Session>::iterator it;
				it = m_robot.find(robot_sn);
				if (it != m_robot.end())
				{
					/* reply heart beat */
					Json::Value heart_beat;
					heart_beat["robot_sn"] = robot_sn;
					heart_beat["key"] = key + 1;
					//heart_beat["soft_version"] = soft_version;

					std::string strValue = heart_beat.toStyledString();
					//TRACE("reply heartbeat with json body: %s", strValue.c_str());
					SendMessage(fd, strValue, ROBOT_HEART_BEAT, 0);
					{
						AutoLock<ThreadMutexLock> lock_(&m_lock);
						/* store heart beat time */
						time_t timer;
						time(&timer);
						(*it).second.time = timer;
					}
				}
				else
				{
					Json::Value request;
					request["status"] = 1;
					request["error_msg"] = "robot doesn't login";

					std::string strValue = request.toStyledString();
					//TRACE("reply user with json body: %s", strValue.c_str());

					SendMessage(fd, strValue, ROBOT_HEART_BEAT, 1);

					/* close the invalid connection */
					sleep(100);
					close(fd);
				}
			}
			else
			{
				//close the fd.
			}
		}
		else
		{
			TRACE("fail to parse json");
		}CATCH_(ServerErrorException, "ProcessRobotHeartBeat failed");

	TRACE("End\n");
}

void MsgHandler::ProcessData(const Param& param)
{
	//TRACE("Begin");
	int tran_code = param.tran_code;
	int body_len = param.data_len;
	TRY_
		if (body_len > 0) {
			switch (tran_code) {
			case USER_LOGIN:
				ProcessUserLogin(param);
				break;
			case HEART_BEAT:
				ProcessRobotHeartBeat(param);
				break;
			default:
				THROW_E(ServerErrorException, "invalid message");
				break;
			}

		}CATCH_(ServerErrorException, "ProcessData failed");
	//TRACE("End");
}

int MsgHandler::RecvData(int fd)
{
	//TRACE("Begin");
	//	int flag = 0;
	int count = 0;

	TRY_
		if (BUFFER_LEN > m_node[fd].recv_len) {
			count = recv(fd, m_node[fd].buf + m_node[fd].recv_len, BUFFER_LEN
					- m_node[fd].recv_len, 0);
		} else {
			TRACE("Error: BUFFER_LEN <= m_node[fd].recv_len");
			count = 0;
		}

		if (count > 0) {
			m_node[fd].recv_len += count;
			//			flag = 1;
		} else if ((count == 0) /*&& (flag == 0)*/) {
			TRACE("client closed");
			ClearSessions(fd);
			close(fd);
			return count;
		} else if (count == -1) {
			TRACE("recv error:%s", strerror(errno));
		}

		while (m_node[fd].recv_len > HEADER_LEN) {
			MessageHeader *header = (MessageHeader*) m_node[fd].buf;
			int body_len = ntohl(header->body_len);
			int tran_code = ntohl(header->tran_code);

			if (m_node[fd].recv_len >= body_len + HEADER_LEN) {
				Param param;
				param.fd = fd;
				param.pData = m_node[fd].buf + HEADER_LEN;
				param.tran_code = tran_code;
				param.data_len = body_len;

				TRACE("tran_code:%d, body_len:%d", tran_code, body_len);

				if (tran_code == 0 && body_len == 0) {
					break;
				}

				ProcessData(param);
				m_node[fd].recv_len -= (HEADER_LEN + body_len);
				memmove(m_node[fd].buf, m_node[fd].buf + HEADER_LEN + body_len,
						m_node[fd].recv_len);
			} else if (body_len + HEADER_LEN > BUFFER_LEN) {
				TRACE("invalid body length");
				ClearSessions(fd);
				close(fd);
				break;
			}
		}CATCH_(ServerErrorException, "RecvData failed");
	//TRACE("End\n");
	return count;
}

void MsgHandler::ClearSessions(int fd)
{

}

int MsgHandler::RecvUDPData(int fd)
{
	MessageHeader *header;
	unsigned int body_len;
	unsigned int tran_code;

	struct sockaddr_in remote;
	int addrlen = sizeof(remote);
	char buf[2048];
	memset(buf, 0, 2048);
	bzero((char*)&remote, sizeof(remote));
	int count = recvfrom(fd, buf, 2048, 0, (struct sockaddr *) &remote,
		(socklen_t *)&addrlen);

	if (count > 0)
	{
		if (buf != NULL)
		{
			header = (MessageHeader*)buf;
			body_len = ntohl(header->body_len);
			tran_code = ntohl(header->tran_code);
			TRACE("tran_code:%d, body_len:%d", tran_code, body_len);
		}
		Param param;
		param.fd = fd;
		param.pData = buf + HEADER_LEN;
		param.tran_code = tran_code;
		param.data_len = body_len;
		param.wan_addr = remote;
		TRACE("wanip:%s, wanport:%ld", inet_ntoa(remote.sin_addr),
			ntohs(remote.sin_port));
		ProcessData(param);
	}

	return count;
}

void MsgHandler::SendMessage(int fd, const string& json, int tranCode,
		int retCode, int version, int checkCode)
{
	MessageHeader msg_header;
	memset(&msg_header, 0, HEADER_LEN);

	msg_header.tran_code = htonl(tranCode);
	msg_header.retcode = htonl(retCode);
	msg_header.version = htonl(version);
	msg_header.check_code = htonl(checkCode);

	const char *tmp = NULL;
	int tmpLen = 0;
	if (!json.empty())
	{
		tmp = json.c_str();
		tmpLen = strlen(tmp);
	}

	msg_header.body_len = htonl(tmpLen);
	int total_len = sizeof(msg_header)+tmpLen;
	char *q = new char[total_len];
	memset(q, 0, total_len);
	memcpy(q, (char*)&msg_header, sizeof(msg_header));

	if (tmpLen > 0)
	{
		memcpy(q + sizeof(msg_header), tmp, tmpLen);
	}

	TRACE("send msg fd: %d", fd);
	if (fd != 0 && (send(fd, (char*)q, sizeof(msg_header)+tmpLen, 0) == -1))
	{
		TRACE("send error: %s", strerror(errno));
	}
	if (q != NULL)
	{
		delete[] q;
		q = NULL;
	}
}

void MsgHandler::SendUDPMessage(const Param& param)
{
	MessageHeader msg_header;
	memset(&msg_header, 0, HEADER_LEN);

	msg_header.tran_code = htonl(param.tran_code);

	int tmpLen = strlen(param.pData);
	msg_header.body_len = htonl(tmpLen);
	int total_len = sizeof(msg_header)+tmpLen;
	char *q = new char[total_len];
	memset(q, 0, total_len);
	memcpy(q, (char*)&msg_header, sizeof(msg_header));

	if (tmpLen > 0)
	{
		memcpy(q + sizeof(msg_header), param.pData, tmpLen);
	}

	int addrlen = sizeof(struct sockaddr_in);
	sendto(param.fd, q, sizeof(msg_header)+tmpLen, 0,
		(sockaddr*)&param.wan_addr, (socklen_t)addrlen);

	if (q != NULL)
	{
		delete[] q;
		q = NULL;
	}
}

void MsgHandler::GetClientAddr(int fd, char* addr)
{

}
