#include "util/plugin.h"
#include <iostream>



int main() {
  auto plugin = epiphiyllum::PluginRegister::getInstance().get_plugin("TestPlugin");
  plugin->run();
  return 0;
}