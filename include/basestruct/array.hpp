#pragma once
#include <iostream>
#include <vector>



namespace epiphiyllum {

template <typename T>
class Array {
public:
  Array(T* data, std::vector<int> shape);
  ~Array();
  template <typename U>
  friend std::ostream& operator<<(std::ostream& os, const Array<U>& array);

private:
  T* data_;                 // 数据指针
  std::vector<int> shape_;  // 矩阵形状
};

template <typename T>
Array<T>::Array(T* data, std::vector<int> shape) {
  std::swap(shape_, shape);
  int size = 1;
  for (auto s : shape_) size *= s;
  data_ = (T*)malloc(size * sizeof(T));
  std::copy(data, data + size, data_);
}

template <typename T>
Array<T>::~Array() {
  free(data_);
}

template <typename U>
std::ostream& operator<<(std::ostream& os, const Array<U>& array) {
  int size = 1;
  for (auto s : array.shape_) size *= s;
  for (int i = 0; i < size - 1; i++) {
    os << *(array.data_ + i) << ",";
  }
  os << *(array.data_ + size - 1);
  return os;
}

}  // namespace epiphiyllum