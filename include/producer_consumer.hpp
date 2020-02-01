// Copyright 2020 Usman Turkaev
#pragma once
#include <chrono>
#include <mutex>
#include <safe_queue.hpp>
#include <thread>

template <class T>
class producer_consumer {
 public:
  producer_consumer() = default;

  producer_consumer(const producer_consumer&) = delete;

  producer_consumer(producer_consumer&& prod_cons) {
    this->queue_ = std::move(prod_cons).queue_;
  }

  ~producer_consumer() = default;

  producer_consumer<T>& operator=(const producer_consumer&) = delete;

  producer_consumer<T>& operator=(producer_consumer& prod_cons) {
    this->queue_ = std::move(prod_cons).queue_;

    return *this;
  }

  void produce(const T& task) { this->queue_.push(task); }

  T consume() {
    T value;
    bool success = false;
    while (this.queue_.try_pop(value) != true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    return value;
  }

 private:
  safe_queue<T> queue_;
};
