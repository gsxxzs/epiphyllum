#include "util/system_util.h"



namespace epiphiyllum{
namespace util{

char* get_process(){
  char process_name[1024];
  readlink("/proc/self/exe", process_name, 1024);
  int pos = 0;
  for(size_t i = 0; i < strlen(process_name); i++)
    if(process_name[i] == '/')  pos = i;
  process_name[strlen(process_name)] = '\0';
  return process_name + pos + 1;
}

} // util
} // epiphiyllum