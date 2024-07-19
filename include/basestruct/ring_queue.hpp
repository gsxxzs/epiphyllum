#pragma once

#include <vector>

#include "util/log.h"



namespace epiphiyllum {

template <typename T>
class RingQueue {
public:
  RingQueue(size_t size = 10) : size_(size + 1), head_(0), tail_(0) { data_.resize(size_); }

  void push(T data) {
    if (this->full()) {
      LOG(WARN) << "queue is full";
      return;
    }
    data_[tail_] = data;
    tail_ = (tail_ + 1) % size_;
  }

  std::optional<T> pop() {
    if (this->empty()) {
      LOG(WARN) << "queue is empty";
      return std::nullopt;
    }
    T data = data_[head_];
    head_ = (head_ + 1) % size_;
    return data;
  }

  bool empty() { return head_ == tail_; }

  bool full() { return (tail_ + 1) % size_ == head_; }

  size_t size() { return size_; }

private:
  std::vector<T> data_;
  size_t size_;
  size_t head_;
  size_t tail_;
};
}  // namespace epiphiyllum
