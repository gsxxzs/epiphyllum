#include <iostream>
#include <thread>
#include <unistd.h>
#include <util/log.h>

int main(){
  epiphiyllum::Config config;
  log_init(config);
  LOG_INFO << "hello world " << 1;
  LOG_WARN << "hello world " << 2;
  LOG_ERROR << "hello world " << 3;
  return 0;
}