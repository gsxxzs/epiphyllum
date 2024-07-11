#include <iostream>
#include <thread>
#include <unistd.h>
#include <util/log.h>
// #include "yaml-cpp/yaml.h"

int main(){
  // std::string input = R"(  
  //       a: 1  
  //       b:  
  //         c: 2  
  //         d: [3, 4]  
  //   )";  
  
  // auto doc = YAML::Load(input);

  epiphiyllum::Config config;
  log_init(config);
  LOG_WARN << "hello world";
  // LOG_INFO << doc["a"].as<std::string>();
  return 0;
}