#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
class ThreadPool {
public:
  ThreadPool(std::size_t num_threads);

  template <class F> void enqueue(F &&f) {
    {
      std::lock_guard<std::mutex> lock(queue_mutex);
      tasks.emplace([this, task = std::forward<F>(f)]() {
        ++active_tasks;
        task();
        --active_tasks;

        all_tasks_completed_condition.notify_all();
      });
    }

    task_added_condition.notify_one();
  }

  void wait_all() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    all_tasks_completed_condition.wait(
        lock, [this]() { return tasks.empty() && active_tasks == 0; });
  }

  ~ThreadPool();

private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;
  std::mutex queue_mutex;
  std::condition_variable task_added_condition;
  bool stop{false};

  std::atomic<int> active_tasks{0};
  std::condition_variable all_tasks_completed_condition;
};
