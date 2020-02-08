// Copyright 2020 Usman Turkaev
#pragma once
#include <chrono>
#include <mutex>
#include <safe_deque.hpp>
#include <thread>

template <class T>
class producer_consumer {
 public:
  producer_consumer() = default;

  producer_consumer(const producer_consumer&) = delete;

  producer_consumer(producer_consumer&& prod_cons) {
    this->deque_ = std::move(prod_cons).deque_;
  }

  ~producer_consumer() = default;

  producer_consumer<T>& operator=(const producer_consumer&) = delete;

  producer_consumer<T>& operator=(producer_consumer& prod_cons) {
    this->deque_ = std::move(prod_cons).deque_;

    return *this;
  }

  void produce(const T& task) { this->deque_.push_back(task); }

  void produce(T&& task) { this->deque_.push_back(task); }

  void produce(std::vector<T>&& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
      this->produce(vec[i]);
    }
  }

  void produce(const std::vector<T>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
      this->produce(vec[i]);
    }
  }

  T consume() {
    T value;
    bool success = false;
    while (this->deque_.try_pop(value) != true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    return value;
  }

  size_t size() { return this->deque_.size(); }

  T& front() { return this->deque_.front(); }

  T& back() { return this->deque_.back(); }

  bool empty() { return this->deque_.empty(); }

  T& operator[](size_t pos) { return this->deque_[pos]; }

  void clear() { this.deque_.clear(); }

  typename std::deque<T>::iterator begin() { return this->deque_.begin(); }

  typename std::deque<T>::iterator end() { return this.deque_.end(); }

 private:
  safe_deque<T> deque_;
};
