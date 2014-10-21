#include "Server.h"
#include "HeartBeatController.h"

#define DEFALT_TIMEOUT		60
#define DEFALT_INTERVAL		5

// public
HeartBeatController::HeartBeatController(MsgHandler* handler) :
	m_handler(handler), m_timeout(DEFALT_TIMEOUT), m_interval(DEFALT_INTERVAL)
{

}

// public
HeartBeatController::~HeartBeatController(void)
{

}

void HeartBeatController::Start(void)
{
	TRY_THROW
		if (pthread_create(&m_thr, NULL, CheckTimeout, this) != 0) {
			THROW_E(HeartBeatControllerException, "fail to create heartbeat controller");
		}CATCH_THROW(HeartBeatControllerException, "Start failed");
}

void HeartBeatController::Stop(void)
{
	TRY_THROW
		if (pthread_join(m_thr, NULL) != 0) {
			THROW_E(HeartBeatControllerException, "fail to stop heartbeat controller");
		}CATCH_THROW(HeartBeatControllerException, "Stop failed");
}

void HeartBeatController::SetTimeout(int timeout)
{
	m_timeout = timeout;
}

void HeartBeatController::SetInterval(int interval)
{
	m_interval = interval;
}

void*
HeartBeatController::CheckTimeout(void* args)
{
	return NULL;
}
