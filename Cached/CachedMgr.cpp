//
//  @ File Name : CachedMgr.cpp 
//  @ Date : 2014/6/5
//  @ Author : bin.li@flyingwings.cn
//

#include "Cached.h"
#include "CachedMgr.h"

DEFINE_THREADSAFE_SINGLETON(CachedMgr)

// public
CachedMgr::CachedMgr(void)
	: _handlers()
	, _servers()
	, _behaviours()
	, _lock()
{
	SetBehavior(MEMCACHED_BEHAVIOR_DISTRIBUTION, MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA);
	// ʹ�ö�����Э�鴫��
	SetBehavior(MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, true);
	// ʹ��Consistent hashing�㷨��key���䵽��ͬ�����Ĳ�ͬλ��
	SetBehavior(MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED, true);
	// ʹ��������tcp
	SetBehavior(MEMCACHED_BEHAVIOR_TCP_NODELAY, true);
	// ����cas֧��
	SetBehavior(MEMCACHED_BEHAVIOR_SUPPORT_CAS, true);
}

// public
CachedMgr::~CachedMgr(void)
{
	AutoLock<ThreadMutexLock> lock(&_lock);
	_handlers.clear();
	_servers.clear();
	_behaviours.clear();
}

// public
uint32
CachedMgr::GetServerCount(void)
{
	return _servers.size();
}

// public
void
CachedMgr::GetServers(vector<string>& servers)
{
	servers.clear();
	set<pair<string, uint32> >::iterator it = _servers.begin();
	for (; it != _servers.end(); ++it) {
		if (!it->first.empty()) {
			servers.push_back(it->first);
		}
	}
}

// public
//boost::shared_ptr<Cached>
std::tr1::shared_ptr<Cached>
CachedMgr::GetHandler(void)
{
	AutoLock<ThreadMutexLock> lock(&_lock);
	pthread_t threadId = pthread_self();
	if (threadId == 0)
		threadId = 323213;

	unordered_map<int64, std::tr1::shared_ptr<Cached>  >::iterator it = _handlers.find((int64)threadId);
	if (it != _handlers.end()) {
		return it->second;
	}

	std::tr1::shared_ptr<Cached> handler =std::tr1::shared_ptr<Cached> (new Cached);
	InitCached(handler.get());
	_handlers.insert(make_pair((int64)threadId, handler));
	return handler;
}

// public
bool
CachedMgr::AddServer(const string& hostIP, const uint32 port)
{
	AutoLock<ThreadMutexLock> lock(&_lock);
	if (_servers.count(make_pair(hostIP, port))) {
		return false;
	}
	_servers.insert(make_pair(hostIP, port));

	unordered_map<int64, std::tr1::shared_ptr<Cached>  >::iterator it = _handlers.begin();
	for (; it != _handlers.end(); ++it) {
		if (!it->second->AddServer(hostIP, port)) {
			return false;
		}
	}

	return true;
}

// public
bool
CachedMgr::SetBehavior(CACHED_BEHAVIOR flag, uint64 data)
{
	AutoLock<ThreadMutexLock> lock(&_lock);
	_behaviours[flag] = data;

	unordered_map<int64, std::tr1::shared_ptr<Cached>  >::iterator it = _handlers.begin();
	for (; it != _handlers.end(); ++it) {
		if (!it->second->SetBehavior(flag, data)) {
			return false;
		}
	}

	return true;
}

// public
uint64
CachedMgr::GetBehavior(CACHED_BEHAVIOR flag)
{
	AutoLock<ThreadMutexLock> lock(&_lock);
	map<CACHED_BEHAVIOR, uint64>::iterator iter = _behaviours.find(flag);
	return iter == _behaviours.end() ? 0 : iter->second;
}

// public
void
CachedMgr::SetServers(const vector<string>& serversList)
{
	AutoLock<ThreadMutexLock> lock(&_lock);
	ClearHandlers();
	ClearServers();

	if (serversList.empty()) {
		return;
	}

	for (size_t i = 0; i < serversList.size(); ++i) {
		AddServer(serversList[i], CACHED_SERVER_PORT);
	}
}

// private
void
CachedMgr::ClearHandlers(void)
{
	_handlers.clear();
}

// private
void
CachedMgr::ClearServers(void)
{
	_servers.clear();
}

// private
void
CachedMgr::InitCached(Cached* cached)
{
	// ������� server
	set<pair<string, uint32> >::iterator its = _servers.begin();
	for (; its != _servers.end(); ++its) {
		cached->AddServer(its->first, its->second);
	}

	// ������� behaviour
	map<CACHED_BEHAVIOR, uint64>::iterator itb = _behaviours.begin();
	for (; itb != _behaviours.end(); ++itb) {
		cached->SetBehavior(itb->first, itb->second);
	}
}
