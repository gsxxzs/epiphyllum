#include <iostream>
#include <vector>
#include <algorithm>
#include "basestruct/array.hpp"

int main(){
  std::vector<int> data {1, 2, 3, 4, 5, 6, 7, 8, 9};
  epiphiyllum::Array<int> array = {data.data(), {3,3}};

  
  std::cout << array << std::endl;
  return 0;
}