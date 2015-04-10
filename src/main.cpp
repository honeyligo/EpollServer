/****************************************************************************************************
main.cpp

Purpose:
	main method

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#include <iostream>

#include "../Common/Common.h"
#include "Server.h"

#define TCP_PORT 26299
#define UDP_PORT 26351
#define THREAD 5
#define QUEUE  5
#define TIMEOUT                                         30
#define INTERVAL                                        5

#define CONFIG_PATH "/opt/config.json"

int main()
{
	Server* srv = NULL;
	try {
		int tcp_port;
		int udp_port;

		int pool_size;
		int pool_task;
		int timeout;
		int interval;

		if (chmod(CONFIG_PATH, 0744) == -1) {
			TRACE("Fail to change mode of config file");
			return -1;
		}

		std::ifstream is;
		is.open(CONFIG_PATH, std::ios::binary);
		char s[1023];
		memset(s, 0, 1023);
		is.read(s, 1023);

		Json::Reader reader;
		Json::Value value;
		if (reader.parse(s, value)) {
			tcp_port = value["tcp_port"].asInt();
			udp_port = value["udp_port"].asInt();

			pool_size = value["pool_size"].asInt();
			pool_task = value["pool_task"].asInt();
			timeout = value["timeout"].asInt();
			interval = value["interval"].asInt();

			tcp_port = (tcp_port == 0) ? TCP_PORT : tcp_port;
			udp_port = (udp_port == 0) ? UDP_PORT : udp_port;
			pool_size = (pool_size == 0) ? THREAD : pool_size;
			pool_task = (pool_task == 0) ? QUEUE : pool_task;
			timeout = (timeout == 0) ? TIMEOUT : timeout;
			interval = (interval == 0) ? INTERVAL : interval;

			TRACE("tcp_port: %d, udp_port: %d, pool_size: %d,  pool_task: %d, timeout: %d, interval: %d",
					tcp_port, udp_port, pool_size, pool_task, timeout, interval);
		} else {
			ServerErrorException e(__FILE__, __LINE__,
					"fail to parse json config");
			throw e;
		}

		is.close();

		srv = new Server(tcp_port, udp_port, pool_size, pool_task, timeout,
				interval);
		srv->Start();
	} catch (ServerErrorException& e) {
		TRACE("ServerErrorException: %s\n", e.what());
		srv->Stop();
	} catch (Exception& e) {
		TRACE("exception: %s\n", e.what());
		srv->Stop();
	} catch (...) {
		TRACE("unknown exception\n");
		srv->Stop();
	}

	if (srv != NULL)
		delete srv;

	return 0;
}
