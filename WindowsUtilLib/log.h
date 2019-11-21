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
	void info(const char* info, ...) {
		va_list arguments;
		va_start(arguments, info);
		char* convertedInfo = str::FmtV(info, arguments);
		defer{ Allocator::Free(nullptr,convertedInfo); };

		ForwardStr(convertedInfo, LogType::Info);

		va_end(arguments);

	}
	void error(const char* info) {
		ForwardStr(info, LogType::Error);
	}

private:
	const char* fileName;
	std::ofstream fd;
	char* GetDateInfo() {
		using namespace std;
		time_t now = time(0);
		//产生“YYYY-MM-DD hh:mm:ss”格式的字符串。
		char s[32];
		tm tmmm;
		localtime_s(&tmmm, &now);
		strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S",&tmmm);
		return str::Dup(s);
	}
	char* GetLogTypeInfo(LogType type) {
		switch (type)
		{
		case SimpleLogger::Info:
			return str::Dup(" [info] ");
		case SimpleLogger::Error:
			return str::Dup(" [error] ");
		case SimpleLogger::Critical:
			return str::Dup(" [critical] ");
		default:
			return nullptr;
		}
	}
	void ForwardStr(const char* info, LogType logType) {
		int infoLen = str::Len(info);

		char* date = GetDateInfo();
		char* logTypeStr = GetLogTypeInfo(logType);
		defer{ Allocator::Free(nullptr,date);
		Allocator::Free(nullptr, logTypeStr);
		};
		str::Str<char> totalMessage;
		totalMessage += date;
		totalMessage += logTypeStr;
		totalMessage += info;

		fd << totalMessage.Get()<<std::endl;
		fd.flush();
	}
};

extern std::shared_ptr<SimpleLogger> file_logger;
void FileLoggerInit(const char* fileName);
#endif // SPDLOG
