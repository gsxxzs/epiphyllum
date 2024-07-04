#include <iostream>
#include <util/log.h>

int main(){
  epiphiyllum::Config config;
  log_init(config);
  LOG_WARN << "hello world";
  return 0;
}