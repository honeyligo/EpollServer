/****************************************************************************************************
CachedMgr.h

Purpose:
	CachedMgr.h

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#ifndef __CACHED__MGR__H__
#define __CACHED__MGR__H__

#include <tr1/memory>

#include "../Common/Common.h"
#include "../Common/Lock.h"
#include "cache.h"


typedef memcached_behavior_t						CACHED_BEHAVIOR;
#define CACHED_SERVER_PORT							11211

class Cached;
class ThreadMutexLock;


class CachedMgr
{
	DECLARE_THREADSAFE_SINGLETON(CachedMgr)

public:
	CachedMgr(void);
	~CachedMgr(void);

	/**
	* Get a specific handler of current thread.
	*/
	std::tr1::shared_ptr<Cached>  GetHandler(void);

	uint32 GetServerCount(void);
	void GetServers(vector<string>& servers);
	bool AddServer(const string& hostIP, const uint32 port);
	bool SetBehavior(CACHED_BEHAVIOR flag, uint64 data);
	uint64 GetBehavior(CACHED_BEHAVIOR flag);
	void SetServers(const vector<string>& serversList);

private:
	void InitCached(Cached* cached);
	void ClearHandlers(void);
	void ClearServers(void);

private:
	unordered_map<int64, std::tr1::shared_ptr<Cached> > _handlers;
	set<pair<string, uint32> > _servers;
	map<CACHED_BEHAVIOR, uint64> _behaviours;
	ThreadMutexLock _lock;
};






#endif //__CACHED__MGR__H__
