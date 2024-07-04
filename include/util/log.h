#pragma once
#include <fstream>
#include <iostream>
#include <string>

#include "util/config.h"

// 定义颜色的宏
#define ANSI_COLOR_RED     "\e[31m"
#define ANSI_COLOR_GREEN   "\e[32m"
#define ANSI_COLOR_YELLOW  "\e[33m"
#define ANSI_COLOR_BLUE    "\e[34m"
#define ANSI_COLOR_RESET   "\e[0m"

#define NOW_TIMESTAMP ""

namespace epiphiyllum{

enum class Level{
  INFO,
  WARN,
  ERROR
};

class LogMessage{
public:
  void init(Config config);
  LogMessage& operator<<(const std::string& str);
  LogMessage& operator()(const Level& level);
  void flush();
  ~LogMessage();

private:
  std::string header_;
  std::string buff_;

  bool is_write_;
  bool color_;
  std::ofstream writer_;
};

class Log{
public:
  void init(Config config){
    message_.init(config);
  }

  Log& operator()(const Level& level){
    message_(level);
    return *this;
  }

  Log& operator<<(const std::string& str){
    message_ << str;
    message_.flush();
    return *this;
  }
private:
  LogMessage message_;
};

static Log logger;

#define LOG(level) epiphiyllum::logger(level)

#define LOG_INFO LOG(epiphiyllum::Level::INFO)
#define LOG_WARN LOG(epiphiyllum::Level::WARN)
#define LOG_ERROR LOG(epiphiyllum::Level::ERROR)

}

void log_init(epiphiyllum::Config config){
  epiphiyllum::logger.init(config);
}

