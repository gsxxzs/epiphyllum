#include "util/log.h"
#include <algorithm>



void log_init(epiphiyllum::Config config){
  // todo 增加log的配置文件
  epiphiyllum::log_level = epiphiyllum::Level::INFO;
  epiphiyllum::log_write_ = false;
  if(epiphiyllum::log_write_){
    epiphiyllum::default_log_name = std::string("log_") + std::string(epiphiyllum::util::get_process()) + ".log";
    std::ofstream writer(epiphiyllum::default_log_name);
    writer.close();
  }
}

namespace epiphiyllum{

LogMessage& LogMessage::operator<<(const std::string& str){
  buff_ += str;
  return *this;
}

LogMessage::LogMessage(const Level& level){
  switch(level){
    case Level::INFO: header_ += "[I"; break;
    case Level::WARN: header_ += "[W"; break;
    case Level::ERROR: header_ += "[E"; break;
  }
}

Log::~Log(){
  auto pos = std::find(file_name_.rbegin(), file_name_.rend(), '/');
  std::string header = message_.get_header() + NOW_TIMESTAMP + "] [" + file_name_.substr(file_name_.rend() - pos) + ":" + std::to_string(line_number_)+ " " + func_+ "]";
  this->write(header + NOW_TIMESTAMP + " " + message_.get_buff());

}

void Log::write(const std::string& str){
  std::string color = "";
  switch(str[1]){
    case 'I': color = ANSI_COLOR_GREEN; break;
    case 'W': color = ANSI_COLOR_YELLOW; break;
    case 'E': color = ANSI_COLOR_RED; break;
  }
  std::cout << color << str << ANSI_COLOR_RESET << std::endl;

  if(log_write_){
    writer_.open(default_log_name, std::ios::app | std::ios::binary);
    writer_ << str << std::endl;
  }
}

}