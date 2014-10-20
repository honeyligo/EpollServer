/****************************************************************************************************
Lock.cpp

Purpose:
	provide RAII style locker implementation

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#include "Lock.h"


// public ctor
ThreadMutexLock::ThreadMutexLock(void)
{
        pthread_mutexattr_init(&m_attr);
        pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m_Mutex, &m_attr);
}

// public dtor
ThreadMutexLock::~ThreadMutexLock(void)
{
	pthread_mutex_destroy(&m_Mutex);
	pthread_mutexattr_destroy(&m_attr);
}

// public virtual
void
ThreadMutexLock::lock(void)
{
	pthread_mutex_lock(&m_Mutex);
}

// public virtual
void
ThreadMutexLock::unlock(void)
{
	pthread_mutex_unlock(&m_Mutex);
}

