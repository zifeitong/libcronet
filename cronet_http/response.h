#ifndef CRONET_HTTP_RESPONSE_H_
#define CRONET_HTTP_RESPONSE_H_

#include <future>
#include <string>

#include "cronet_c.h"

namespace cronet_http {

class Response {
 public:
  Response();

  int32_t http_status_code() { return http_status_code_; };
  std::string http_status_text() { return http_status_text_; };

 private:
  friend class Client;

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

  int32_t http_status_code_;
  std::string http_status_text_;

  Cronet_UrlRequestCallbackPtr const callback_;
};

}  // namespace cronet_http

#endif  // CRONET_HTTP_RESPONSE_H_
