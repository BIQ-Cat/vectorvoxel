#include "thread_pool.h"
#include <cstddef>
#include <functional>
#include <mutex>
#include <utility>

ThreadPool::ThreadPool(std::size_t num_threads) {
  for (std::size_t i{}; i < num_threads; ++i) {
    workers.emplace_back([this]() {
      while (true) {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(queue_mutex);
          task_added_condition.wait(
              lock, [this]() { return stop || !tasks.empty(); });

          if (stop && tasks.empty())
            return;

          task = std::move(tasks.front());
          tasks.pop();
        }

        task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  {
    std::lock_guard<std::mutex> lock(queue_mutex);
    stop = true;
  }

  task_added_condition.notify_all();
  for (auto &worker : workers) {
    if (worker.joinable())
      worker.join();
  }
}
