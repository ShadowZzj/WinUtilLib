#include "log.h"

std::shared_ptr<spdlog::logger> file_logger;
void FileLoggerInit(const char* fileName) {
	file_logger = spdlog::basic_logger_mt("log", fileName);
	spdlog::set_default_logger(file_logger);
	spdlog::flush_every(std::chrono::seconds(2));
}