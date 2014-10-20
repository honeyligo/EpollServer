/****************************************************************************************************
Common.cpp

Purpose:
	provide common header files and public methods

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#include "Common.h"


Logger* Logger::instance = NULL;
ThreadMutexLock* Logger::lock = NULL;
ofstream Logger::ofstr;

Logger* Logger::GetInstance(void)
{
	if (instance == NULL) {
		lock = new ThreadMutexLock;
		AutoLock<ThreadMutexLock> lck(lock);
		instance = new Logger;

		int rt = ::access(LOG_PATH, F_OK);

		if (rt != 0) {
			::chmod(LOG_PATH, 0777);

			//#ifdef 	DEBUG
			//				::mkdir(LOG_PATH, 0777);
			//#else
			char cmd[512] = { };
			sprintf(
					cmd,
					"sh -c echo \"libin123\" | sudo -S sh -c \"sudo mkdir -p %s\"",
					LOG_PATH);
			TRACE("%s",cmd);
			system(cmd);
			//#endif
		}

		char file[1024] = { };
		time_t timep;
		struct tm *p;
		time(&timep);
		p = localtime(&timep);
		::snprintf(file, sizeof(file), "%s/%d-%d-%d.log", LOG_PATH, 1900
				+ p->tm_year, 1 + p->tm_mon, p->tm_mday);

		ofstr.open(file, ios::binary | ios::app);

		atexit(DelInstance);
	}
	return instance;
}

void Logger::DelInstance(void)
{
	if (Logger::lock != NULL)
		delete Logger::lock;
	if (instance != NULL) {
		Logger::ofstr.close();
		delete instance;
	}
}

std::string Logger::DateFormat()
{
	/* 格式化时间 */
	time_t timep;
	struct tm *p;

	AutoLock<ThreadMutexLock> lck(lock);

	time(&timep);
	p = localtime(&timep);

	ostringstream date;
	date << 1900 + p->tm_year << "/" << 1 + p->tm_mon << "/" << p->tm_mday
			<< " " << p->tm_hour << ":" << p->tm_min << ":" << p->tm_sec;

	return date.str();
}

void Logger::Write(const char* fileName, int line, const char* func,
		const char *format, ...)
{
	/* 打印日志信息 */
	va_list arg;
	char tmp[1024];
	va_start(arg, format);
	vsprintf(tmp, format, arg);
	va_end(arg);

	ostringstream logstream;
	logstream << DateFormat() << "  " << fileName << ":" << line << " " << func
			<< " -- " << tmp << "\n" << "\n";

	AutoLock<ThreadMutexLock> lck(lock);
	cout << logstream.str();

	/* save to file */
	ofstr << logstream.str();
	ofstr.flush();
}

CurlTool::response CurlTool::Post(const std::string& url,
		const std::string& data, const std::string& auth)
{
	TRACE("url: %s, data: %s", url.c_str(), data.c_str());

	CURLcode ret;
	CURL *curl = curl_easy_init();

	CurlTool::response res;
	CurlTool::request req;
	req.data = data.c_str();
	req.len = data.size();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, req.len);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.data);
		curl_easy_setopt(curl, CURLOPT_POST, 1); /* use post method */
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH , CURLAUTH_BASIC);
		curl_easy_setopt(curl, CURLOPT_USERPWD , auth.c_str());
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res);

		ret = curl_easy_perform(curl);
		if (ret != CURLE_OK) {
			TRACE("curl_easy_perform() failed: %s",curl_easy_strerror(ret));
		}
	}

	curl_easy_cleanup(curl);

	return res;
}

size_t CurlTool::WriteCallback(void *data, size_t size, size_t nmemb,
		void *userData)
{
	CurlTool::response* r;
	r = reinterpret_cast<CurlTool::response*> (userData);
	r->body.append(reinterpret_cast<char*> (data), size * nmemb);

	return (size * nmemb);
}

