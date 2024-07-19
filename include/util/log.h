#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "util/config.h"
#include "util/system_util.h"



// 定义颜色的宏
#define ANSI_COLOR_RED     "\e[31m"
#define ANSI_COLOR_GREEN   "\e[32m"
#define ANSI_COLOR_YELLOW  "\e[33m"
#define ANSI_COLOR_BLUE    "\e[34m"
#define ANSI_COLOR_RESET   "\e[0m"

#define NOW_TIMESTAMP ""

namespace epiphiyllum {

enum class Level { INFO, WARN, ERROR };

Level log_level = Level::INFO;
bool log_write_ = false;
std::string default_log_name = "log";

class LogMessage {
public:
  LogMessage &operator<<(const std::string &str);

  template <typename T>
  LogMessage &operator<<(const T &value) {
    std::ostringstream oss;
    oss << value;
    *this << oss.str();
    return *this;
  }

  LogMessage() {}
  LogMessage(const Level &level);
  ~LogMessage() {}

  std::string get_header() { return header_; }
  std::string get_buff() { return buff_; }

private:
  std::string header_;
  std::string buff_;
};

class Log {
public:
  Log(const Level &level, std::string file_name, int line_number, std::string func)
      : message_(LogMessage(level)), file_name_(file_name), line_number_(line_number), func_(func) {}
  ~Log();
  void write(const std::string &str);
  LogMessage &stream() { return message_; };

private:
  std::ofstream writer_;
  LogMessage message_;
  std::string file_name_;
  int line_number_;
  std::string func_;
};
}  // namespace epiphiyllum

#define LOG_INFO                                          \
  if (epiphiyllum::Level::INFO >= epiphiyllum::log_level) \
  epiphiyllum::Log(epiphiyllum::Level::INFO, __FILE__, __LINE__, __func__).stream()
#define LOG_WARN                                          \
  if (epiphiyllum::Level::WARN >= epiphiyllum::log_level) \
  epiphiyllum::Log(epiphiyllum::Level::WARN, __FILE__, __LINE__, __func__).stream()
#define LOG_ERROR                                          \
  if (epiphiyllum::Level::ERROR >= epiphiyllum::log_level) \
  epiphiyllum::Log(epiphiyllum::Level::ERROR, __FILE__, __LINE__, __func__).stream()

#define LOG(level) LOG_##level

void log_init(epiphiyllum::Config config);
