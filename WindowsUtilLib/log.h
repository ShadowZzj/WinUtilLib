#pragma once
#ifdef SPDLOG
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
extern std::shared_ptr<spdlog::logger> file_logger;

void FileLoggerInit(const char* fileName);
#else
#include "BaseUtil.h"
#include <fstream>
#include <ctime>
class SimpleLogger;
extern std::shared_ptr<SimpleLogger> file_logger;
void FileLoggerInit(const char* fileName);

class SimpleLogger {
public:
	enum LogType {
		Info,
		Error,
		Critical
	};
	SimpleLogger(const char* fileName = nullptr) :fileName(fileName) {
		if (str::Len(fileName)) {
			fd.open(fileName, std::ios_base::out | std::ios_base::app);
		}
		else
			CrashMe();
	}
	~SimpleLogger() {
		fd.close();
	}
	void info(const char* info, ...);
	void error(const char* info);

private:
	const char* fileName;
	std::ofstream fd;
	char* GetDateInfo();
	char* GetLogTypeInfo(LogType type);
	void ForwardStr(const char* info, LogType logType);
};
#endif 
