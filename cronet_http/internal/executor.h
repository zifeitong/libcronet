#ifndef CRONET_HTTP_INTERNAL_EXECUTOR_H_
#define CRONET_HTTP_INTERNAL_EXECUTOR_H_

#include <queue>
#include <stop_token>
#include <thread>

#include "absl/synchronization/mutex.h"
#include "cronet_c.h"
#include "cronet_http/internal/memory.h"

namespace cronet_http::internal {

class Executor {
 public:
  Executor();
  ~Executor();

  Cronet_ExecutorPtr GetExecutor();

 private:
  bool IsTaskAvailable() const;

  void ThreadLoop(std::stop_token stop_token);

  void Execute(Cronet_RunnablePtr runnable);
  static void Execute(Cronet_ExecutorPtr self, Cronet_RunnablePtr runnable);

  mutable absl::Mutex mutex_;
  std::queue<Cronet_RunnablePtr> task_queue_;
  MAKE_CRONET_C_UNIQUE_PTR_WITH(Cronet_Executor, executor_, Executor::Execute);
  std::jthread executor_thread_;
};

}  // namespace cronet_http::internal

#endif  // CRONET_HTTP_INTERNAL_EXECUTOR_H_
