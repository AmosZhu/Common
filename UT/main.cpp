/*
 * main.cpp
 *
 *  Created on: May 29, 2016
 *      Author: amos
 */

#include "Logger.hpp"
#include "pthread.h"
#include <errno.h>
#include <string.h>

#define DEBUG_TYPE (0)
#define RELEASE_TYPE (1)

#define VERSION DEBUG_TYPE



#if (VERSION==DEBUG_TYPE)
	static Logger::Logger<Logger::file_log_policy> logger("test.log");
	#define LOG_DEBUG(args...) logger.print<Logger::DEBUG>("[Func:",__FUNCTION__,", Line:",__LINE__,"] ",##args)
	#define LOG_WARNING(args...) logger.print<Logger::WARNING>("[Func:",__FUNCTION__,", Line:",__LINE__,"] ",##args)
	#define LOG_ERROR(args...) logger.print<Logger::ERROR>("[Func:",__FUNCTION__,", Line:",__LINE__,"] ",##args)
#else
	#define LOG_DEBUG
	#define LOG_WARNING
	#define LOG_ERROR
#endif

void* func(void* arg) {
	int num = 10000;
	for (int i = 0; i < num; i++) {
		LOG_DEBUG("Hello, this is test debug,does it work fine?");
		LOG_WARNING("Warning, warning, going to test!");
		LOG_ERROR("What happened if error revoked!!!");
	}
	return nullptr;
}

int main(int argc, char *argv[]) {
	int num = 10;
	pthread_t tid[10];

	for (int i = 0; i < num; i++) {
		int err;
		err = pthread_create(&tid[i], nullptr, func, nullptr); //创建线程
		if (err != 0) {
			printf("create thread error: %s/n", strerror(err));
			return 1;
		}
	}

	for (int i = 0; i < num; i++) {
		int err;
		err = pthread_join(tid[i], nullptr);
		if (err != 0) {
			printf("create thread error: %s/n", strerror(err));
			return 1;
		}
	}

	return 1;
}

