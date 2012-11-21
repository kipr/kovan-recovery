#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

#ifndef __linux__
#define LOG_PERROR(format, arg...) do {} while(0)

#else

#define LOG_PERROR(format, arg...) \
	do { \
		fprintf(stderr, "%s - %s():%d - " format ": %s\n", __FILE__, __func__, __LINE__, ## arg, strerror(errno)); \
	} while(0)
#endif

#define LOG_ERROR(format, arg...) \
	do { \
		fprintf(stderr, "%s - %s():%d - " format "\n", __FILE__, __func__, __LINE__, ## arg); \
	} while(0)

#define LOG_NOTE(format, arg...) \
	do { \
		fprintf(stderr, "%s - %s():%d - " format "\n", __FILE__, __func__, __LINE__, ## arg); \
	} while(0)

#endif
