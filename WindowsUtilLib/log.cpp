#include "log.h"
//Any cpp include log.h and one need to call FileLoggerInit.
#ifdef SPDLOG
std::shared_ptr<spdlog::logger> file_logger;
bool isfile_loggerInit = false;

void FileLoggerInit(const char* fileName) {
	if (isfile_loggerInit) {
		return;
	}
	file_logger = spdlog::basic_logger_mt("log", fileName);
	spdlog::set_default_logger(file_logger);
	spdlog::flush_every(std::chrono::seconds(2));
	isfile_loggerInit = true;
	return;
}
#else
std::shared_ptr<SimpleLogger> file_logger;
bool isfile_loggerInit = false;


void FileLoggerInit(const char* fileName) {
	if (isfile_loggerInit) {
		return;
	}
	file_logger = std::make_shared<SimpleLogger>(fileName);
	isfile_loggerInit = true;
}

void SimpleLogger::info(const char* info, ...) {
	va_list arguments;
	va_start(arguments, info);
	char* convertedInfo = str::FmtV(info, arguments);
	defer{ Allocator::Free(nullptr,convertedInfo); };

	ForwardStr(convertedInfo, LogType::Info);

	va_end(arguments);

}

void SimpleLogger::error(const char* info) {
	ForwardStr(info, LogType::Error);
}

char* SimpleLogger::GetDateInfo() {
	using namespace std;
	time_t now = time(0);
	//产生“YYYY-MM-DD hh:mm:ss”格式的字符串。
	char s[32];
	tm tmmm;
	localtime_s(&tmmm, &now);
	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tmmm);
	return str::Dup(s);
}

char* SimpleLogger::GetLogTypeInfo(LogType type) {
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

void  SimpleLogger::ForwardStr(const char* info, LogType logType) {
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

	fd << totalMessage.Get() << std::endl;
	fd.flush();
}
#endif