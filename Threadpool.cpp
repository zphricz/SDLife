#include "Threadpool.h"

void detach_threads() {
  Singleton<Threadpool>::instance().detach_threads();
}

void wait_for_all_jobs() {
  Singleton<Threadpool>::instance().wait_for_all_jobs();
}

bool all_jobs_complete() {
  return Singleton<Threadpool>::instance().all_jobs_complete();
}
