#include "../Common/Lock.h"
#include "MsgHandler.h"
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
TRY_
  time_t currTime;
  HeartBeatController* ctrl = (HeartBeatController*) args;

  if(ctrl == NULL || ctrl->m_handler == NULL)
    THROW_E(HeartBeatControllerException, "fail to get heartbeat controller");

  while (1)
    {
     //  check every 5 second
      sleep(5);

      // compare time
      time(&currTime);


      AutoLock<ThreadMutexLock> lock(&ctrl->m_handler->m_lock);
      std::map<string, Session>* robot = &(ctrl->m_handler->m_robot);

      for (std::map<string, Session>::iterator it = robot->begin(); it != robot->end(); )
        {
          if (currTime - (*it).second.time > ctrl->m_timeout)
            {
              TRACE("robot[%s]'s fd:%d timeouts", (*it).first.c_str(), (*it).second.fd);

              memset(&(ctrl->m_handler->m_node[(*it).second.fd]), 0, sizeof(FdDataNode));

              // clear power of user
             std::map<string, Session>::iterator it_user = ctrl->m_handler->m_user.find((*it).second.user_name);
             if(it_user != ctrl->m_handler->m_user.end())
               {
                 (*it_user).second.power = 0;
                 (*it_user).second.udp_received = false;
               }

             // clear all data of robot
              robot->erase(it++);
            }
          else
            {
              //AutoLock<ThreadMutexLock> lock(&ctrl->m_handler->m_lock);
              it++;
            }
        }
    }
  CATCH_(HeartBeatControllerException, "check timeout failed");

  pthread_exit(NULL);
  return (NULL);
}
