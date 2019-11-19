#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
extern std::shared_ptr<spdlog::logger> file_logger;

void FileLoggerInit(const char* fileName);