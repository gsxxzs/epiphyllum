#include "util/log.h"



namespace epiphiyllum{

void LogMessage::init(Config config){
  // to do 实现config接口，外部传参数进行初始化
  is_write_ = true;
  color_ = true;
  writer_.open("./log.log");
}

LogMessage& LogMessage::operator<<(const std::string& str){
  buff_ += str;
  return *this;
}

LogMessage& LogMessage::operator()(const Level& level){
  std::string color = "";
  switch(level){
    case Level::INFO:  
      color = ANSI_COLOR_GREEN; 
      header_ += "[I] "; 
      break;
    case Level::WARN: 
      color = ANSI_COLOR_YELLOW;
      header_ += "[W] ";
      break;
    case Level::ERROR:
      color =  ANSI_COLOR_RED; 
      header_ += "[E] "; 
      break;
  }
  if(color_) header_ = color + header_;
  return *this;
}

void LogMessage::flush(){
  std::cout << header_ << NOW_TIMESTAMP << buff_ << std::endl;
  if(is_write_){
    writer_ << header_ << NOW_TIMESTAMP << buff_ << std::endl;
  }
}

LogMessage::~LogMessage(){
  if(is_write_){
    writer_.close();
  }
}

}