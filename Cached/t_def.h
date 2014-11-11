/****************************************************************************************************
t_def.h

Purpose:
	type define header

Author:
	Wookong

Created Time:
	2014-6-5
****************************************************************************************************/

#ifndef DEF_H
#define DEF_H

#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#define MAX_BUF_SIZE 2048
#define ERROR_BUF_SIZE 65535
#define MAX_LINE 1024
#define TEST_NUM 128


#ifndef uchar
typedef unsigned char uchar;
#endif

#ifndef u8
typedef uchar u8;
#endif

#ifndef ushort
typedef unsigned short ushort;
#endif

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef ulong
typedef unsigned long ulong;
#endif

#ifndef ulonglong
typedef unsigned long long ulonglong;
#endif

#ifndef uint8_t
#define uint8_t uchar
#endif

#ifndef uint8
#define uint8 uint8_t
#endif

#ifndef uint16_t
#define uint16_t ushort
#endif

#ifndef uint16
#define uint16 uint16_t
#endif

#ifndef uint32_t
#define uint32_t uint
#endif

#ifndef uint32
#define uint32 uint32_t
#endif

#ifndef uint64_t
#define uint64_t ulonglong
#endif

#ifndef uint64
#define uint64 uint64_t
#endif

#ifndef int8_t
#define int8_t char
#endif

#ifndef int8
#define int8 int8_t
#endif

#ifndef int16_t
#define int16_t short
#endif

#ifndef int16
#define int16 int16_t
#endif

#ifndef int32_t
#define int32_t int
#endif

#ifndef int32
#define int32 int32_t
#endif

#ifndef int64_t
#define int64_t long long
#endif

#ifndef int64
#define int64 int64_t
#endif



#endif // DEF_H
