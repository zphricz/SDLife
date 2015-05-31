#include "Threadpool.h"

void Threadpool::set_num_threads(int new_num_threads) {
  Singleton<Pool>::instance().set_num_threads(new_num_threads);
}

int Threadpool::get_num_threads() {
  return Singleton<Pool>::instance().get_num_threads();
}

void Threadpool::detach_threads() {
  Singleton<Pool>::instance().detach_threads();
}

void Threadpool::wait_for_all_jobs() {
  Singleton<Pool>::instance().wait_for_all_jobs();
}

bool Threadpool::all_jobs_complete() {
  return Singleton<Pool>::instance().all_jobs_complete();
}
