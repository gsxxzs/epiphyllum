#include <algorithm>
#include <iostream>
#include <vector>

#include "basestruct/ring_queue.hpp"



int main() {
  epiphiyllum::RingQueue<int> que{10};
  que.pop();
  for (int i = 0; i < 10; i++) {
    LOG(INFO) << i;
    que.push(i);
  }
  return 0;
}