#include "executor.h"

#include <functional>

namespace cronet_http::internal {

Executor::Executor()
    : executor_thread_(std::bind_front(&Executor::ThreadLoop, this)) {
  Cronet_Executor_SetClientContext(executor_.get(), this);
}

Executor::~Executor() {
  absl::MutexLock lock(mutex_);
  executor_thread_.request_stop();
}

Cronet_ExecutorPtr Executor::GetExecutor() { return executor_.get(); }

bool Executor::IsTaskAvailable() const {
  return !task_queue_.empty() ||
         executor_thread_.get_stop_token().stop_requested();
}

void Executor::ThreadLoop(std::stop_token stop_token) {
  while (true) {
    Cronet_RunnablePtr runnable = nullptr;

    mutex_.LockWhen(absl::Condition(this, &Executor::IsTaskAvailable));

    if (stop_token.stop_requested()) {
      mutex_.unlock();
      break;
    }

    runnable = task_queue_.front();
    task_queue_.pop();
    mutex_.unlock();

    Cronet_Runnable_Run(runnable);
    Cronet_Runnable_Destroy(runnable);
  }

  // Delete remaining tasks.
  std::queue<Cronet_RunnablePtr> tasks_to_destroy;
  {
    absl::MutexLock lock(mutex_);
    tasks_to_destroy.swap(task_queue_);
  }

  while (!tasks_to_destroy.empty()) {
    Cronet_Runnable_Destroy(tasks_to_destroy.front());
    tasks_to_destroy.pop();
  }
}

void Executor::Execute(Cronet_RunnablePtr runnable) {
  {
    absl::MutexLock lock(mutex_);
    if (!executor_thread_.get_stop_token().stop_requested()) {
      task_queue_.push(runnable);
      runnable = nullptr;
    }
  }

  if (runnable) {
    Cronet_Runnable_Destroy(runnable);
  }
}

/* static */
void Executor::Execute(Cronet_ExecutorPtr self, Cronet_RunnablePtr runnable) {
  auto* executor =
      static_cast<Executor*>(Cronet_Executor_GetClientContext(self));
  executor->Execute(runnable);
}

}  // namespace cronet_http::internal
