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
//using namespace ZDB;
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

}

void MsgHandler::ProcessData(const Param& param)
{
	//TRACE("Begin");
	int tran_code = param.tran_code;
	int body_len = param.data_len;
	TRY_
		if (body_len > 0) {
			switch (tran_code) {
			//case USER_LOGIN:
			//	ProcessUserLogin(param);
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
			unsigned int body_len = ntohl(header->body_len);
			unsigned int tran_code = ntohl(header->tran_code);

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

}

void MsgHandler::SendMessage(int fd, const string& json, int tranCode,
		int retCode, int version, int checkCode)
{

}

void MsgHandler::SendUDPMessage(const Param& param)
{

}

void MsgHandler::GetClientAddr(int fd, char* addr)
{

}
