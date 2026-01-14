#ifndef CRONET_HTTP_RESPONSE_H_
#define CRONET_HTTP_RESPONSE_H_

#include <string>

#include "absl/base/thread_annotations.h"
#include "absl/synchronization/mutex.h"
#include "cronet_c.h"
#include "cronet_http/internal/memory.h"

namespace cronet_http {

class Response {
 public:
  ~Response();

  int32_t http_status_code() { return http_status_code_; };
  std::string http_status_text() { return http_status_text_; };

  bool Read(const std::byte** data, size_t* bytes_read);

 private:
  static constexpr int kBufferSize = 32 * 1024;

  friend class Client;

  enum class State {
    kNew,
    kStarted,
    kSucceeded,
    kCancelled,
    kFailed,
  };

  Response();

  // Wait until the request is started.
  void WaitUntilStarted() const;
  bool IsStarted() const;

  // Wait until the request is completed (succeeded, failed or cancelled).
  void WaitUntilCompleted() const;
  bool IsCompleted() const;

  // Wait until the read is completed (succeeded or finished).
  bool IsReadCompleted() const;

  // Implementation of Cronet_UrlRequestCallback interface.
  void OnRedirectReceived(Cronet_UrlRequestPtr request,
                          Cronet_UrlResponseInfoPtr info,
                          Cronet_String newLocationUrl);
  static void OnRedirectReceived(Cronet_UrlRequestCallbackPtr self,
                                 Cronet_UrlRequestPtr request,
                                 Cronet_UrlResponseInfoPtr info,
                                 Cronet_String newLocationUrl);

  void OnResponseStarted(Cronet_UrlRequestPtr request,
                         Cronet_UrlResponseInfoPtr info);
  static void OnResponseStarted(Cronet_UrlRequestCallbackPtr self,
                                Cronet_UrlRequestPtr request,
                                Cronet_UrlResponseInfoPtr info);

  void OnReadCompleted(Cronet_UrlRequestPtr request,
                       Cronet_UrlResponseInfoPtr info, Cronet_BufferPtr buffer,
                       uint64_t bytes_read);
  static void OnReadCompleted(Cronet_UrlRequestCallbackPtr self,
                              Cronet_UrlRequestPtr request,
                              Cronet_UrlResponseInfoPtr info,
                              Cronet_BufferPtr buffer, uint64_t bytes_read);

  void OnSucceeded(Cronet_UrlRequestPtr request,
                   Cronet_UrlResponseInfoPtr info);
  static void OnSucceeded(Cronet_UrlRequestCallbackPtr self,
                          Cronet_UrlRequestPtr request,
                          Cronet_UrlResponseInfoPtr info);

  void OnFailed(Cronet_UrlRequestPtr request, Cronet_UrlResponseInfoPtr info,
                Cronet_ErrorPtr error);
  static void OnFailed(Cronet_UrlRequestCallbackPtr self,
                       Cronet_UrlRequestPtr request,
                       Cronet_UrlResponseInfoPtr info, Cronet_ErrorPtr error);

  void OnCanceled(Cronet_UrlRequestPtr request, Cronet_UrlResponseInfoPtr info);
  static void OnCanceled(Cronet_UrlRequestCallbackPtr self,
                         Cronet_UrlRequestPtr request,
                         Cronet_UrlResponseInfoPtr info);

  // Response data.
  int32_t http_status_code_;
  std::string http_status_text_;

  // Internal states.
  MAKE_CRONET_C_UNIQUE_PTR(Cronet_UrlRequest, request_);
  Cronet_BufferPtr buffer_;
  Cronet_UrlRequestCallbackPtr const callback_;

  mutable absl::Mutex mutex_;
  State state_ ABSL_GUARDED_BY(mutex_) = State::kNew;
  bool ready_to_read_ ABSL_GUARDED_BY(mutex_) = false;
  size_t last_bytes_read_ ABSL_GUARDED_BY(mutex_) = 0;
};

}  // namespace cronet_http

#endif  // CRONET_HTTP_RESPONSE_H_
