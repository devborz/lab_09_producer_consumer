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

  inline void produce(const T& value) { this->deque_.push_back(value); }

  inline void produce(T&& value) { this->deque_.push_back(value); }

  void produce(std::vector<T>&& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
      if (!this->check_existance(vec[i])) this->produce(vec[i]);
    }
  }

  void produce(const std::vector<T>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
      if (!this->check_existance(vec[i])) this->produce(vec[i]);
    }
  }

  inline T consume() { return this->deque_.pop_front(); }

  inline size_t size() { return this->deque_.size(); }

  inline T& front() { return this->deque_.front(); }

  inline T& back() { return this->deque_.back(); }

  inline bool empty() { return this->deque_.empty(); }

  inline T& operator[](size_t pos) { return this->deque_[pos]; }

  inline void clear() { this.deque_.clear(); }

  inline bool check_existance(const T& value) {
    return this->deque_.check_existance(value);
  }

  inline void start_producing() {
    if (!this->is_producing_) this->is_producing_ = true;
  }

  inline void stop_producing() {
    if (this->is_producing_) this->is_producing_ = false;
  }

  inline bool is_producing() { return this->is_producing_; }

  inline typename std::deque<T>::iterator begin() {
    return this->deque_.begin();
  }

  inline typename std::deque<T>::iterator end() { return this->deque_.end(); }

 private:
  bool is_producing_ = false;
  safe_deque<T> deque_;
};
