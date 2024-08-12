#pragma once
#include <vector>
#include <functional>
#include <unordered_map>
#include <iostream>



namespace epiphiyllum {

class BasePlugin {
public:
  virtual ~BasePlugin() = default;
  virtual void init() = 0;
  virtual void run() = 0;
  virtual void config() = 0;
  virtual void clear() = 0;
};

class PluginRegister{
public:
  static PluginRegister& getInstance() {
    static PluginRegister instance;
    return instance;
  }

  PluginRegister(const PluginRegister&) = delete;
  PluginRegister& operator=(const PluginRegister&) = delete;

  void add(std::string plugin_name, std::function<BasePlugin*()> fun){
    if(plugin_registers_.find(plugin_name) == plugin_registers_.end()){
      plugin_registers_[plugin_name] = fun;
    }
  }
  BasePlugin* get_plugin(std::string plugin_name){
    if (plugin_registers_.find(plugin_name) != plugin_registers_.end()) {
      return plugin_registers_[plugin_name]();
    }
    return nullptr;
  }
private:
  PluginRegister() = default; 
  std::unordered_map<std::string, std::function<BasePlugin*()>> plugin_registers_;
};

#define REGISTER_PLUGIN(name_space, class_name) \
  static const bool class_name##registered = []() { \
      epiphiyllum::PluginRegister& instance = epiphiyllum::PluginRegister::getInstance(); \
      instance.add(#class_name, []() -> epiphiyllum::BasePlugin* { return new name_space::class_name(); }); \
      return true; \
    }();
  
}  // namespace epiphiyllum