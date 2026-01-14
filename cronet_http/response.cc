#include "cronet_http/response.h"

namespace cronet_http {

namespace {

Response* GetThis(Cronet_UrlRequestCallbackPtr self) {
  return static_cast<Response*>(
      Cronet_UrlRequestCallback_GetClientContext(self));
}

}  // namespace

Response::Response()
    : callback_(Cronet_UrlRequestCallback_CreateWith(
          Response::OnRedirectReceived, Response::OnResponseStarted,
          Response::OnReadCompleted, Response::OnSucceeded, Response::OnFailed,
          Response::OnCanceled)) {
  Cronet_UrlRequestCallback_SetClientContext(callback_, this);
  // Cronet_Buffer lifetime is managed by the cronet internally.
  MAKE_CRONET_C_UNIQUE_PTR(Cronet_Buffer, buffer);
  buffer_ = buffer.release();
  Cronet_Buffer_InitWithAlloc(buffer_, kBufferSize);
}

Response::~Response() {
  if (!IsCompleted()) {
    Cronet_UrlRequest_Cancel(request_.get());
  }
  WaitUntilCompleted();
}

bool Response::Read(const std::byte** data, size_t* bytes_read) {
  Cronet_UrlRequest_Read(request_.get(), buffer_);

  mutex_.LockWhen(absl::Condition(this, &Response::IsReadCompleted));
  if (IsCompleted()) {
    mutex_.Unlock();
    return false;
  }
  ready_to_read_ = false;
  mutex_.Unlock();

  *data = static_cast<const std::byte*>(Cronet_Buffer_GetData(buffer_));
  *bytes_read = last_bytes_read_;
  return true;
}

bool Response::IsReadCompleted() const {
  return IsCompleted() || ready_to_read_;
};

void Response::WaitUntilStarted() const {
  mutex_.LockWhen(absl::Condition(this, &Response::IsStarted));
  mutex_.Unlock();
};

bool Response::IsStarted() const { return state_ != State::kNew; };

void Response::WaitUntilCompleted() const {
  mutex_.LockWhen(absl::Condition(this, &Response::IsCompleted));
  mutex_.Unlock();
};

bool Response::IsCompleted() const {
  return (state_ == State::kSucceeded || state_ == State::kFailed ||
          state_ == State::kCancelled);
}

void Response::OnRedirectReceived(Cronet_UrlRequestPtr request,
                                  Cronet_UrlResponseInfoPtr info,
                                  Cronet_String newLocationUrl) {
  Cronet_UrlRequest_FollowRedirect(request);
}

void Response::OnResponseStarted(Cronet_UrlRequestPtr request,
                                 Cronet_UrlResponseInfoPtr info) {
  http_status_code_ = Cronet_UrlResponseInfo_http_status_code_get(info);
  http_status_text_ = Cronet_UrlResponseInfo_http_status_text_get(info);

  {
    absl::MutexLock lock(&mutex_);
    state_ = State::kStarted;
  }
}

void Response::OnReadCompleted(Cronet_UrlRequestPtr request,
                               Cronet_UrlResponseInfoPtr info,
                               Cronet_BufferPtr buffer, uint64_t bytes_read) {
  {
    absl::MutexLock lock(&mutex_);
    last_bytes_read_ = bytes_read;
    ready_to_read_ = true;
  }
}

void Response::OnSucceeded(Cronet_UrlRequestPtr request,
                           Cronet_UrlResponseInfoPtr info) {
  absl::MutexLock lock(&mutex_);
  state_ = State::kSucceeded;
}

void Response::OnFailed(Cronet_UrlRequestPtr request,
                        Cronet_UrlResponseInfoPtr info, Cronet_ErrorPtr error) {
  absl::MutexLock lock(&mutex_);
  state_ = State::kFailed;
}

void Response::OnCanceled(Cronet_UrlRequestPtr request,
                          Cronet_UrlResponseInfoPtr info) {
  absl::MutexLock lock(&mutex_);
  state_ = State::kCancelled;
}

/* static */ void Response::OnRedirectReceived(
    Cronet_UrlRequestCallbackPtr self, Cronet_UrlRequestPtr request,
    Cronet_UrlResponseInfoPtr info, Cronet_String newLocationUrl) {
  GetThis(self)->OnRedirectReceived(request, info, newLocationUrl);
}

/* static */ void Response::OnResponseStarted(Cronet_UrlRequestCallbackPtr self,
                                              Cronet_UrlRequestPtr request,
                                              Cronet_UrlResponseInfoPtr info) {
  GetThis(self)->OnResponseStarted(request, info);
}

/* static */ void Response::OnReadCompleted(Cronet_UrlRequestCallbackPtr self,
                                            Cronet_UrlRequestPtr request,
                                            Cronet_UrlResponseInfoPtr info,
                                            Cronet_BufferPtr buffer,
                                            uint64_t bytes_read) {
  GetThis(self)->OnReadCompleted(request, info, buffer, bytes_read);
}

/* static */ void Response::OnSucceeded(Cronet_UrlRequestCallbackPtr self,
                                        Cronet_UrlRequestPtr request,
                                        Cronet_UrlResponseInfoPtr info) {
  GetThis(self)->OnSucceeded(request, info);
}

/* static */ void Response::OnFailed(Cronet_UrlRequestCallbackPtr self,
                                     Cronet_UrlRequestPtr request,
                                     Cronet_UrlResponseInfoPtr info,
                                     Cronet_ErrorPtr error) {
  GetThis(self)->OnFailed(request, info, error);
}

/* static */ void Response::OnCanceled(Cronet_UrlRequestCallbackPtr self,
                                       Cronet_UrlRequestPtr request,
                                       Cronet_UrlResponseInfoPtr info) {
  GetThis(self)->OnCanceled(request, info);
}

}  // namespace cronet_http
