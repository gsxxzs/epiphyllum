#include "util/plugin.h"
#include <iostream>


namespace test {

class TestPlugin: public epiphiyllum::BasePlugin{
public:
  TestPlugin() = default;
  virtual void init(){};
  virtual void run(){
    std::cout << "test plugin runing ......" << std::endl;
  };
  virtual void config(){};
  virtual void clear(){};
};

REGISTER_PLUGIN(test, TestPlugin);
}