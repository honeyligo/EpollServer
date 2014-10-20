//
//  @ File Name : common.h 
//  @ Date : 2014/6/5
//  @ Author : bin.li@flyingwings.cn
//


#ifndef __COMMON__H__
#define __COMMON__H__

#include <inttypes.h>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <libmemcached/memcached.h>

#include "t_def.h"




typedef memcached_behavior_t CACHED_BEHAVIOR;


#define DECLARE_SINGLETON(_class)							\
	public:													\
	static _class* getInstance(void);						\
	static void delInstance(void);							\
	private:												\
	static _class* _mgrInst;

#define DEFINE_SINGLETON(_class)							\
	_class* _class::_mgrInst = 0;							\
	_class* _class::getInstance(void)						\
	{														\
		if (_class::_mgrInst == 0) {						\
			_class::_mgrInst = new _class;					\
		}													\
		return _class::_mgrInst;							\
	}														\
	void _class::delInstance(void)							\
	{														\
		if (_class::_mgrInst != 0) {						\
															\
				delete _mgrInst;							\
				_class::_mgrInst = 0;						\
		}													\
	}


#define DECLARE_THREADSAFE_SINGLETON(_class)				\
	public:													\
	static _class* getInstance(void);						\
	static void delInstance(void);							\
	private:												\
	static _class* _mgrInst;								\
	static ThreadMutexLock _mgrInstLock;

#define DEFINE_THREADSAFE_SINGLETON(_class)					\
	_class* _class::_mgrInst = 0;							\
	ThreadMutexLock _class::_mgrInstLock;					\
	_class* _class::getInstance(void)						\
	{														\
		if (_class::_mgrInst == 0) {						\
			AutoLock<ThreadMutexLock> lock(&_mgrInstLock);	\
			/* double-check lock */							\
			if (_class::_mgrInst == 0) {					\
					_class::_mgrInst = new _class;			\
			}												\
		}													\
		return _class::_mgrInst;							\
	}														\
	void _class::delInstance(void)							\
	{														\
		AutoLock<ThreadMutexLock> lock(&_mgrInstLock);		\
		if (_class::_mgrInst != 0) {						\
			delete _mgrInst;								\
			_class::_mgrInst = 0;							\
		}													\
}



#endif //__COMMON__H__
