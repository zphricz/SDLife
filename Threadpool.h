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

namespace Threadpool {
  /*
   * This is an implementation of a simple threadpool. You can specify the
   * number of threads for it to run in the constructor, or by using the
   * set_num_threads() function. All jobs submitted to the threadpool will be
   * inserted into a global queue that is synchronized by a single global
   * mutex. You can submit jobs with the submit_contract() function to have
   * a std::future<T> as a return value, or with the submit_task() function if
   * you do not care about the future and want to minimize overhead.
   *
   * TODO:
   *   - If you submit a job that expects a reference argument (e.g.
   *     void f(int&)) in order to get correct behavior, the value passed in
   *     needs to be wrapped in std::ref. However, the code will still compile
   *     if you don't wrap in std::ref. It will just behave incorrectly at
   *     runtime. Fix this.
   *   - submit_contract does not work for jobs that are templated with clang
   *     (e.g. T add_two_of_generic_type<T>(T arg1, T arg2). Fix this?
   *   - submit_task does not work for jobs that don't return void with clang.
   *     Fix this?
   *   - Implement thread-local queues and job-stealing?
   */
  class Pool {
    public:
    explicit Pool()
      : running_threads(0), num_threads(std::thread::hardware_concurrency()) {
      start_threads();
    }

    explicit Pool(int num_threads)
      : running_threads(0), num_threads(num_threads) {
      if (num_threads <= 0) {
        num_threads = 1;
      }
      start_threads();
    }

    Pool(Pool &other) = delete;
    Pool(const Pool &other) = delete;
    Pool(Pool &&other) = delete;
   
    // The destructor will cancel any remaining jobs. Make sure to call
    // wait_for_all_jobs() before letting the destructor execute
    ~Pool() {
      reap_threads();
    }

    // There is a lot of overhead in this function. First all threads are told
    // to stop running, then they are joined, then an entirely new set of
    // threads are spawned. Only run this function when needed
    void set_num_threads(int new_num_threads) {
      if (new_num_threads <= 0) {
        new_num_threads = 1;
      }
      if (new_num_threads != num_threads) {
        reap_threads();
        threads.clear();
        threads.shrink_to_fit();
        num_threads = new_num_threads;
        start_threads();
      }
    }

    int get_num_threads() {
      return num_threads;
    }

    void detach_threads() {
      for (auto &thread : threads) {
        if (thread.joinable()) {
          thread.detach();
        }
      }
    }

    // Blocks until all jobs are complete
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
      running = true;
      threads.reserve(num_threads);
      for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(&Pool::thread_loop, this);
      }
    }

    // Joins all threads without waiting for all jobs being completed
    void reap_threads() {
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

    void thread_loop() {
      std::function<void()>job;
      {
        std::lock_guard<std::mutex> lk(m);
        running_threads++;
      }
      while (true) {
        {
          std::unique_lock<std::mutex> lk(m);
          running_threads--;
          if (job_queue.empty() && running_threads == 0) {
            signal_main.notify_one();
          }
          signal_threads.wait(lk, [&] {
            return !job_queue.empty() || !running;
          });
          if (!running) {
            break;
          }
          running_threads++;
          job = std::move(job_queue.front());
          job_queue.pop_front();
        }
        job();
      }
    }

    std::mutex m;
    std::deque<std::function<void()>> job_queue;
    int running_threads; // Number of threads performing jobs
    bool running;
    int num_threads; // Number of threads running in this Threadpool
    std::condition_variable signal_threads; // Used to wake up threads
    std::condition_variable signal_main;  // Used to wake up main
    std::vector<std::thread> threads;
  };

  void set_num_threads(int new_num_threads);

  int get_num_threads();

  void detach_threads();

  void wait_for_all_jobs();

  bool all_jobs_complete();

  // Use submit_task() for tasks that need as little overhead as possible
  template <typename F, typename... Args>
  void submit_task(F &&f, Args &&... args) {
    Singleton<Pool>::instance().submit_task(std::forward<F>(f),
                                            std::forward<Args>(args)...);
  }

  template <typename F>
  void submit_task(F &&f) {
    Singleton<Pool>::instance().submit_task(std::forward<F>(f));
  }

  // submit_contract() has more overhead than submit_task(), but also provides
  // a future as a return
  template <typename F, typename... Args>
  std::future<typename std::result_of<F(Args...)>::type>
  submit_contract(F &&f, Args &&... args) {
    return Singleton<Pool>::instance()
      .submit_contract(std::forward<F>(f), std::forward<Args>(args)...);
  }

  template <typename F>
  std::future<typename std::result_of<F()>::type>
  submit_contract(F &&f) {
    return Singleton<Pool>::instance().submit_contract(std::forward<F>(f));
  }
}

#endif 
