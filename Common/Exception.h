/****************************************************************************************************
Exception.h

Purpose:
	provide Exception class

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#if PRAGMA_ONCE
#pragma once
#endif

#include <exception>
#include <string>
#include <sstream>
#include <typeinfo>
#include <execinfo.h>

//#include "../Common/Common.h"

#define	EMPTY_STRING	""

#define THROW_E(exptType, msg) 								\
do {																					\
exptType ___e___(__FILE__, __LINE__, msg); 		\
throw ___e___; 																\
} while (false)

#define TRY_																	\
	try {

/* DO NOT throw exception in CATCH expression */
#define CATCH_(exptType, msg)									\
	}																						\
	catch (exptType& e)													\
	{																						\
		cout << msg << " -- " << e.what() << endl;\
	}																						\
	catch (Exception& e)												\
	{																						\
		cout << e.what() << endl;									\
	}																						\
	catch (...)																	\
	{																						\
		cout <<"Unknown exception\n"<< endl;			\
	}

#define TRY_THROW															\
	try {

/* throw exception in CATCH expression */
#define CATCH_THROW(exptType, msg)						\
	}																						\
	catch (exptType& e)													\
	{																						\
		string str(msg);													\
		str += " -- ";														\
		str += e.getMessage();										\
		exptType ___e___(__FILE__, __LINE__, str);\
		throw ___e___;														\
	}																						\
	catch (Exception& e)												\
	{																						\
		throw;																		\
	}																						\
	catch (...)																	\
	{																						\
		throw;																		\
	}

class Exception: public std::exception {
public:

	Exception(const std::string& fileName, int codeLine, const std::string& msg) throw () :
		_what(EMPTY_STRING), _fileName(fileName), _codeLine(codeLine),
				_msg(msg)
	{
		fillStackTrace();
	}

	/// \brief Create exception object as copy of another
	Exception(const Exception& e) throw () :
		std::exception(e), _what(EMPTY_STRING), _fileName(e._fileName),
				_codeLine(e._codeLine), _msg(e._msg), _stack(e._stack)
	{
	}

	/// \brief Assign another exception object's contents to this one
	Exception& operator=(const Exception& rhs) throw ()
	{
		_what = rhs._what;
		return *this;
	}

	/// \brief Destroy exception object
	virtual ~Exception() throw ()
	{

	}

	/// \brief Returns explanation of why exception was thrown
	virtual const char* what() throw ()
	{
		return toFullString().c_str();
	}

	virtual std::string toFullString(void) throw ()
	{
		std::ostringstream outs;
		outs << " file name : " << _fileName << ", code line: " << _codeLine
				<< ", error message: " << _msg << "\n" << "stack:\n" << _stack
				<< "\n";
		_what = outs.str();

		/* print expt when running*/
		//cout << _what.c_str() << endl;

		return _what.c_str();
	}

	const std::string stackTrace() const throw ()
	{
		return _stack;
	}

	virtual std::string getFileName(void) const throw ()
	{
		return _fileName;
	}

	virtual int getCodeLine(void) const throw ()
	{
		return _codeLine;
	}

	virtual std::string getMessage(void) const throw ()
	{
		return _msg;
	}

	virtual void setFileName(const std::string& fileName) throw ()
	{
		_fileName = fileName;
	}

	virtual void setCodeLine(int codeLine) throw ()
	{
		_codeLine = codeLine;
	}

	virtual void setMessage(const std::string& msg) throw ()
	{
		_msg = msg;
	}

protected:
	/// \brief Create exception object
	Exception(const char* w = EMPTY_STRING) throw () :
		_what(w)
	{
	}

	/// \brief Create exception object
	Exception(const std::string& w) throw () :
		_what(w)
	{
	}

private:

	void fillStackTrace()
	{
		const int len = 200;
		void* buffer[len];
		int nptrs = ::backtrace(buffer, len);
		char** strings = ::backtrace_symbols(buffer, nptrs);
		if (strings) {
			for (int i = 0; i < nptrs; ++i) {
				// TODO demangle funcion name with abi::__cxa_demangle
				_stack.append(strings[i]);
				_stack.push_back('\n');
			}
			free(strings);
		}
	}

protected:
	/// \brief explanation of why exception was thrown
	std::string _what;
	std::string _fileName;
	int _codeLine;
	std::string _msg;
	std::string _stack;
};

class ServerErrorException: public Exception {
public:
	/// \brief Create exception object as copy of another
	ServerErrorException(void) throw () :
		Exception(__FILE__, __LINE__, "ServerErrorException")
	{
	}

	ServerErrorException(const std::string& fileName, int codeLine,
			const std::string& msg) throw () :
		Exception(fileName, codeLine, msg)
	{
	}

	/// \brief Destroy exception object
	virtual ~ServerErrorException() throw ()
	{
	}
};

class DBMiddlewareException: public Exception {
public:
	/// \brief Create exception object as copy of another
	DBMiddlewareException(void) throw () :
		Exception(__FILE__, __LINE__, "DBMiddlewareException")
	{
	}

	DBMiddlewareException(const std::string& fileName, int codeLine,
			const std::string& msg) throw () :
		Exception(fileName, codeLine, msg)
	{
	}

	/// \brief Destroy exception object
	virtual ~DBMiddlewareException() throw ()
	{
	}
};

class MemcachedException: public Exception {
public:
	/// \brief Create exception object as copy of another
	MemcachedException(void) throw () :
		Exception(__FILE__, __LINE__, "MemcachedException")
	{
	}

	MemcachedException(const std::string& fileName, int codeLine,
			const std::string& msg) throw () :
		Exception(fileName, codeLine, msg)
	{
	}

	/// \brief Destroy exception object
	virtual ~MemcachedException() throw ()
	{
	}
};

class HeartBeatControllerException: public Exception {
public:
	/// \brief Create exception object as copy of another
	HeartBeatControllerException(void) throw () :
		Exception(__FILE__, __LINE__, "HeartBeatControllerException")
	{
	}

	HeartBeatControllerException(const std::string& fileName, int codeLine,
			const std::string& msg) throw () :
		Exception(fileName, codeLine, msg)
	{
	}

	/// \brief Destroy exception object
	virtual ~HeartBeatControllerException() throw ()
	{
	}
};

#endif // __EXCEPTION_H__
