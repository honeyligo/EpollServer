/****************************************************************************************************
Lock.h

Purpose:
	provide RAII style locker

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#ifndef __LOCK_H__
#define __LOCK_H__

#if PRAGMA_ONCE
#pragma once
#endif

#include <stdlib.h>
#include <pthread.h>

template <typename lock_t>
struct AutoLock
{
	AutoLock(lock_t* lock)
	: _lock(lock)
	{
		_lock->lock();
	}

	~AutoLock(void)
	{
		_lock->unlock();
	}

	lock_t* _lock;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// class #ThreadMutexLock

class ThreadMutexLock
{
public:
	ThreadMutexLock(void);
	~ThreadMutexLock(void);

	virtual void lock(void);
	virtual void unlock(void);

private:
	pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_attr;
};

#endif //__LOCK_H__
