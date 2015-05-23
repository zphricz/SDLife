#ifndef THREADPOOL_HEADER
#define THREADPOOL_HEADER

#include <deque>
#include <thread>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <functional>

/*
 * TODO:
 *   - There is no way to get return values of jobs submitted. modifiy 
 *     Threadpool to use futures to get return values
 *   - The threadpool fails when member functions are submitted as jobs.
 *     Determine why this is and fix it.
 *   - If you submit a job that expects a reference argument (e.g. void f(int&))
 *     in order to get correct behavior, the value passed in needs to be
 *     wrapped in std::ref. However, the code will still compile if you don't 
 *     wrap in std::ref. It will just behave incorrectly at runtime. Fix this.
 */
class Threadpool {
 public:
  Threadpool() :
    running(true),
    num_threads(recommend_threadcount()) {
    running_threads = num_threads;
    start_threads();
  }

  Threadpool(int num_threads) :
    running(true),
    num_threads(num_threads) {
    if (num_threads <= 0) {
      num_threads = 1;
    }
    running_threads = num_threads;
    start_threads();
  }

  // The destructor will cancel any remaining jobs. Make sure to call
  // wait_for_all_jobs() before letting the destructor execute
  ~Threadpool() {
    {
      std::lock_guard<std::mutex> lk(m);
      running = false;
    }
    signal_threads.notify_all();
    for (auto& thread: threads) {
      thread.join();
    }
  }

  static int recommend_threadcount() {
    return std::thread::hardware_concurrency();
  }

#if 0
  // My attempt at getting Futures implemented. Ignore for now.
  template<class F>
  std::future<typename std::result_of<F()>::type>
  submit_job(F&& f) {
    auto p = new std::promise<typename std::result_of<F()>::type>();
    {
      std::lock_guard<std::mutex> lk(m);
      job_queue.emplace_back([p, f]{
          auto b = f();
          p->set_value(std::move(b));
          delete p;
      });
    }
    signal_threads.notify_one();
    return p->get_future();
  }

  template<class F, class... Args>
  std::future<typename std::result_of<F(Args...)>::type>
  submit_job(F&& f, Args&&... args) {
    auto p = new std::promise<typename std::result_of<F(Args...)>::type>();
    {
      std::lock_guard<std::mutex> lk(m);
      job_queue.emplace_back([p, f, args...]{
          p->set_value(f(args...));
          delete p;
      });
    }
    signal_threads.notify_one();
    return p->get_future();
  }
#else
  template<class F>
  void submit_job(F&& f) {
    {
      std::lock_guard<std::mutex> lk(m);
      job_queue.emplace_back(std::bind(f));
    }
    signal_threads.notify_one();
  }

  template<class F, class... Args>
  void submit_job(F&& f, Args&&... args) {
    {
      std::lock_guard<std::mutex> lk(m);
      job_queue.emplace_back(std::bind(f, args...));
    }
    signal_threads.notify_one();
  }
#endif

#if 0
  // My attempt at getting reference arguments handled correctly. Ignore for
  // now
  template<class F, class... Args>
  void submit_job(F&& f, Args&... args) = delete;
#endif

  void wait_for_all_jobs() {
    std::unique_lock<std::mutex> lk(m);
    signal_main.wait(lk, [&]{
      return job_queue.empty() && running_threads == 0;
    });
  }

  bool all_jobs_complete() {
    std::lock_guard<std::mutex> lk(m);
    return job_queue.empty() && running_threads == 0;
  }

 private:
  void start_threads() {
    for (int i = 0; i < num_threads; i++) {
      threads.push_back(std::thread(&Threadpool::thread_loop, this));
    }
  }

  void thread_loop() {
    std::unique_lock<std::mutex> lk(m);
    while (running) {
      if (job_queue.empty()) {
        running_threads--;
        signal_main.notify_one();
        signal_threads.wait(lk,[&]{return !job_queue.empty() || !running;});
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
  std::condition_variable signal_main; // Used to wake up main
  std::vector<std::thread> threads;

public:
  const int num_threads; // Number of threads running in this Threadpool
};

#endif //THREADPOOL_HEADER
