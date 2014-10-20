//
//  @ File Name : Cached.cpp 
//  @ Date : 2014/6/5
//  @ Author : bin.li@flyingwings.cn
//

//#include <libmemcached/memory.h>


#include "Cached.h"


// public
Cached::Cached(void)
{
	_memc = memcached(0, 0);
}

// public
Cached::~Cached(void)
{
	if (_memc) {
		memcached_free(_memc);
		_memc = 0;
	}
}

// public
bool
Cached::GetStats(ServerStatusMap& statsMap)
{
	statsMap.clear();

	memcached_return_t rc;
	memcached_stat_st *stats = memcached_stat(_memc, 0, &rc);

	if (!memcached_success(rc)) {
		return false;
	}

	uint32 serverCount = GetServerCount();
	string serverName;
	for (uint32 i = 0; i < serverCount; ++i) {
		memcached_server_instance_st instance = const_cast <memcached_server_instance_st> (memcached_server_instance_by_position(_memc, i));
		stringstream ss;
		ss << memcached_server_name(instance) << ":" << memcached_server_port(instance);
		ss >> serverName;

		StatusMap serverStats;
		char **list = memcached_stat_get_keys(_memc, &stats[i], &rc);

		for (char** ptr = list; *ptr; ++ptr) {
			char *value = memcached_stat_get_value(_memc, &stats[i], *ptr, &rc);

			serverStats.insert(make_pair(string(*ptr, strlen(*ptr)), string(value, strlen(value))));

			free(value);
		}
		statsMap.insert(make_pair(serverName, serverStats));
		free(list);
	}

	memcached_stat_free(_memc, stats);
	return true;
}

// public
bool
Cached::Get(const string& cacheKey,
			char*& itemValue,
			int64& itemValueLength)
{
	if (CacheGet(cacheKey, itemValue, itemValueLength) != CachedRetType::SUCCESS) {
		return false;
	}

	return true;
}

// public
bool 
Cached::Set(const string& cacheKey,
			const char* itemValue, 
			int64 itemValueLength)
{
	if (CacheSet(cacheKey, itemValue, itemValueLength, 0) != CachedRetType::SUCCESS) {
		return false;
	}

	return true;
}

// public
bool
Cached::Replace(const string& cacheKey, 
				const char* itemValue, 
				int64 itemValueLength)
{
	if (CacheReplace(cacheKey, itemValue, itemValueLength, 0) != CachedRetType::SUCCESS) {
		return false;
	}

	return true;
}

// public
bool
Cached::Add(const string& cacheKey,
			const char* itemValue,
			int64 itemValueLength)
{
	if (CacheAdd(cacheKey, itemValue, itemValueLength, 0) != CachedRetType::SUCCESS) {
		return false;
	}

	return true;
}

// public
bool 
Cached::Delete(const string& cacheKey)
{
	return (CacheDelete(cacheKey, 0) == CachedRetType::SUCCESS);
}

// public
bool
Cached::Flush(void)
{
	//TRACE(_T("void"));

	return (CacheFlush(time_t(0)) == CachedRetType::SUCCESS);
}

