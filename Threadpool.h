#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <deque>
#include <thread>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <future>

#include "Singleton.h"

/*
 * TODO:
 *   - If you submit a job that expects a reference argument (e.g. void f(int&))
 *     in order to get correct behavior, the value passed in needs to be
 *     wrapped in std::ref. However, the code will still compile if you don't
 *     wrap in std::ref. It will just behave incorrectly at runtime. Fix this.
 *   - submit_contract does not work for jobs that are templated
 *     (e.g. T add_two_of_generic_type<T>(T arg1, T arg2). Fix this.
 *   - Implement Threadpool singleton pattern?
 */
class Threadpool {
  public:
  explicit Threadpool()
    : running(true), num_threads(std::thread::hardware_concurrency()) {
    running_threads = num_threads;
    start_threads();
  }

  explicit Threadpool(int num_threads)
    : running(true), num_threads(num_threads) {
    if (num_threads <= 0) {
      num_threads = 1;
    }
    running_threads = num_threads;
    start_threads();
  }

  Threadpool(Threadpool &other) = delete;
  Threadpool(const Threadpool &other) = delete;
  Threadpool(Threadpool &&other) = delete;
 
  // The destructor will cancel any remaining jobs. Make sure to call
  // wait_for_all_jobs() before letting the destructor execute
  ~Threadpool() {
    {
      std::lock_guard<std::mutex> lk(m);
      running = false;
    }
    signal_threads.notify_all();
    for (auto &thread : threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  // Use submit_task() for tasks that need as little overhead as possible
  template <typename F, typename... Args>
  void submit_task(F &&f, Args &&... args) {
    {
      std::lock_guard<std::mutex> lk(m);
      job_queue.emplace_back(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    }
    signal_threads.notify_one();
  }

  template <typename F>
  void submit_task(F &&f) {
    {
      std::lock_guard<std::mutex> lk(m);
      job_queue.emplace_back(std::forward<F>(f));
    }
    signal_threads.notify_one();
  }

  // submit_contract() has more overhead than submit_task(), but also provides
  // a future as a return
  template <typename F, typename... Args>
  std::future<typename std::result_of<F(Args...)>::type>
  submit_contract(F &&f, Args &&... args) {
    typedef typename std::result_of<F(Args...)>::type R;
    return submit_helper(std::function<R()>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...)));
  }

  template <typename F>
  std::future<typename std::result_of<F()>::type>
  submit_contract(F &&f) {
    typedef typename std::result_of<F()>::type R;
    return submit_helper(std::function<R()>(std::forward<F>(f)));
  }

  void detach_threads() {
    for (auto &thread : threads) {
      thread.detach();
    }
  }

  void wait_for_all_jobs() {
    std::unique_lock<std::mutex> lk(m);
    signal_main.wait(lk, [&] {
      return job_queue.empty() && running_threads == 0;
    });
  }

  bool all_jobs_complete() {
    std::lock_guard<std::mutex> lk(m);
    return job_queue.empty() && running_threads == 0;
  }

  private:
  template <typename R>
  std::future<R> submit_helper(std::function<R()> &&func) {
    auto p = std::make_shared<std::promise<R>>();
    {
      std::lock_guard<std::mutex> lk(m);
      job_queue.emplace_back([p, func] { p->set_value(func()); });
    }
    signal_threads.notify_one();
    return p->get_future();
  }

  std::future<void> submit_helper(std::function<void()> &&func) {
    auto p = std::make_shared<std::promise<void>>();
    {
      std::lock_guard<std::mutex> lk(m);
      job_queue.emplace_back([p, func] {
        func();
        p->set_value();
      });
    }
    signal_threads.notify_one();
    return p->get_future();
  }

  void start_threads() {
    for (int i = 0; i < num_threads; i++) {
      threads.emplace_back(&Threadpool::thread_loop, this);
    }
  }

  void thread_loop() {
    std::unique_lock<std::mutex> lk(m);
    while (running) {
      if (job_queue.empty()) {
        running_threads--;
        if (running_threads == 0) {
          signal_main.notify_one();
        }
        signal_threads.wait(lk, [&] { return !job_queue.empty() || !running; });
        running_threads++;
        if (!running) {
          break;
        }
      }
      auto job = std::move(job_queue.front());
      job_queue.pop_front();
      lk.unlock();
      job();
      lk.lock();
    }
  }

  std::mutex m;
  std::deque<std::function<void()>> job_queue;
  int running_threads; // Number of threads performing jobs
  bool running;
  std::condition_variable signal_threads; // Used to wake up threads
  std::condition_variable signal_main;  // Used to wake up main
  std::vector<std::thread> threads;

  public:
  const int num_threads; // Number of threads running in this Threadpool
};

// Use submit_task() for tasks that need as little overhead as possible
template <typename F, typename... Args>
void submit_task(F &&f, Args &&... args) {
  Singleton<Threadpool>::instance().submit_task(std::forward<F>(f),
                                                std::forward<Args>(args)...);
}

template <typename F>
void submit_task(F &&f) {
  Singleton<Threadpool>::instance().submit_task(std::forward<F>(f));
}

// submit_contract() has more overhead than submit_task(), but also provides
// a future as a return
template <typename F, typename... Args>
std::future<typename std::result_of<F(Args...)>::type>
submit_contract(F &&f, Args &&... args) {
  return Singleton<Threadpool>::instance().submit_contract(std::forward<F>(f),
                                                   std::forward<Args>(args)...);
}

template <typename F>
std::future<typename std::result_of<F()>::type>
submit_contract(F &&f) {
  return Singleton<Threadpool>::instance().submit_contract(std::forward<F>(f));
}

void detach_threads();

void wait_for_all_jobs();

bool all_jobs_complete();

#endif 
