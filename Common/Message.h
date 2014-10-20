/*
 * Message.h
 *
 *  Created on: May 11, 2014
 *      Author: root
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
typedef struct MessageHeader
{
	MessageHeader(void)
	: body_len(0)
	, tran_code(0)
	, version(0)
	, check_code(0)
	, request_id(0)
	, retcode(0)
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

typedef struct Param
{
	Param(void)
	: msg_type(0)
	, data_len(0)
	, fd(0)
	, port(0)
	, tran_code(0)
	, pData(NULL)
	, ip(NULL)
	, wan_addr()
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
#endif /* MESSAGE_H_ */
