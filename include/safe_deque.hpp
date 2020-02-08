// Copyright 2020 Usman Turkaev
#pragma once
#include <deque>
#include <mutex>
#include <vector>

template <class T>
class safe_deque {
 public:
  safe_deque() = default;

  safe_deque(const safe_deque& safe) { this->deque_ = safe.deque_; }

  safe_deque<T>& operator=(const safe_deque& safe) {
    if (this->deque_ != safe.deque_) this->deque_ = safe.deque_;
    return *this;
  }

  safe_deque(safe_deque&& safe) { this->deque_ = std::move(safe).deque_; }

  safe_deque<T>& operator=(safe_deque&& safe) {
    if (this->deque_ != std::move(safe).deque_)
      this->deque_ = std::move(safe).deque_;
    return *this;
  }

  ~safe_deque() = default;

  void push_back(const T& value) {
    std::lock_guard<std::mutex> lock(this->mutex_);
    this->deque_.push_back(value);
  }

  void push_back(T&& value) {
    std::lock_guard<std::mutex> lock(this->mutex_);
    this->deque_.push_back(std::move(value));
  }

  void push_front(const T& value) {
    std::lock_guard<std::mutex> lock(this->mutex_);
    this->deque_.push_front(value);
  }

  void push_front(T&& value) {
    std::lock_guard<std::mutex> lock(this->mutex_);
    this->deque_.push_front(std::move(value));
  }

  T pop_back() {
    std::lock_guard<std::mutex> lock(this->mutex_);
    auto value = this->deque.back();
    this->deque_.pop_back();
    return value;
  }

  T pop_front() {
    std::lock_guard<std::mutex> lock(this->mutex_);
    auto value = this->deque_.front();
    this->deque_.pop_front();
    return value;
  }

  bool try_pop(T& value) {
    if (this->mutex_.try_lock() && !this->deque_.empty()) {
      value = this->pop_front();
      return true;
    }
    return false;
  }

  template <class... Args>
  void emplace(Args&&... args) {
    std::lock_guard<std::mutex> lock(this->mutex_);
    this->deque_.emplace(args...);
  }

  size_t size() { return this->deque_.size(); }

  T& front() {
    std::lock_guard<std::mutex> lock(this->mutex_);
    return this->deque_.front();
  }

  T& back() {
    std::lock_guard<std::mutex> lock(this->mutex_);
    return this->deque_.back();
  }

  bool empty() { return this->deque_.empty(); }

  T& operator[](size_t pos) {
    std::lock_guard<std::mutex> lock(this->mutex_);
    return this->deque_[pos];
  }

  void clear() { this->deque_.clear(); }

  typename std::deque<T>::iterator begin() {
    std::lock_guard<std::mutex> lock(this->mutex_);
    return this->deque_.begin();
  }

  typename std::deque<T>::iterator end() {
    std::lock_guard<std::mutex> lock(this->mutex_);
    return this.deque_.end();
  }

  void resize(size_t size) { this->deque_.resize(size); }

  safe_deque<T>& operator=(std::vector<T>&& vec);

  safe_deque<T>& operator=(const std::vector<T>& vec);

 private:
  std::deque<T> deque_;

  std::mutex mutex_;
};

template <class T>
safe_deque<T>& safe_deque<T>::operator=(std::vector<T>&& vec) {
  if (!this->empty()) {
    this->clear();
  }
  for (size_t i = 0; i < vec.size(); ++i) {
    this->push_back(vec[i]);
  }
  return *this;
}

template <class T>
safe_deque<T>& safe_deque<T>::operator=(const std::vector<T>& vec) {
  if (!this->empty()) {
    this->clear();
  }
  for (size_t i = 0; i < vec.size(); ++i) {
    this->push_back(vec[i]);
  }
  return *this;
}
