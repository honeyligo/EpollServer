#ifndef __HEART_BEAT_CONTROLLER_H__
#define __HEART_BEAT_CONTROLLER_H__

#include "../Common/Common.h"

class MsgHandler;

class HeartBeatController {
public:

	HeartBeatController(MsgHandler* handler);
	virtual ~HeartBeatController(void);

	virtual void Start(void);

	virtual void Stop(void);

	/**
	 * @param timeout: second as unit
	 */
	void SetTimeout(int timeout);

	/**
	 * @param interval: second as unit
	 */
	void SetInterval(int interval);

private:
	static void* CheckTimeout(void* args);

protected:
	MsgHandler* m_handler;

private:
	int m_timeout;
	int m_interval;

	//ThreadMutexLock m_lock;

	pthread_t m_thr;

}; // End class HeartBeatController

#endif // __HEART_BEAT_CONTROLLER_H__
