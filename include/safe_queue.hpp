// Copyright 2020 Usman Turkaev
#pragma once
#include <deque>
#include <mutex>
#include <queue>
template <class T>
class safe_queue {
 public:
  safe_queue() = default;

  safe_queue(const safe_queue& queue) { this->queue_ = queue; }

  safe_queue<T>& operator=(const safe_queue& queue) {
    if (this->queue_ != queue) this->queue_ = queue;

    return *this;
  }

  safe_queue(safe_queue&& queue) { this->queue_ = std::move(queue); }

  safe_queue<T>& operator=(safe_queue&& queue) {
    if (this->queue_ != std::move(queue)) this->queue_ = std::move(queue);

    return *this;
  }

  ~safe_queue() = default;

  void push(const T& value) {
    std::lock_guard<std::mutex> lock(this->mutex_);

    this->queue_.push();
  }

  T pop() {
    std::lock_guard<std::mutex> lock(this->mutex_);

    auto value = this->queue.front();
    this->queue_.pop();
    return value;
  }

  bool try_pop(T& value) {
    if (this->mutex_.try_lock() && !this->queue_.empty()) {
      value = this->pop();
      return true;
    }
    return false;
  }

  template <class... Args>
  void emplace(Args&&... args) {
    std::lock_guard<std::mutex> lock(this->mutex_);

    this->queue_.emplace(args...);
  }

  size_t size() { return this->queue_.size(); }

  T& front() { return this->queue_.front(); }

  T& back() { return this->queue_.back(); }

  bool empty() { return this->queue_.empty(); }

 private:
  std::queue<T> queue_;

  std::mutex mutex_;
};
