#include "cronet_http/response.h"

#include <string_view>

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

  Cronet_BufferPtr buffer = Cronet_Buffer_Create();
  Cronet_Buffer_InitWithAlloc(buffer, 32 * 1024);
  Cronet_UrlRequest_Read(request, buffer);
}

void Response::OnReadCompleted(Cronet_UrlRequestPtr request,
                               Cronet_UrlResponseInfoPtr info,
                               Cronet_BufferPtr buffer, uint64_t bytes_read) {
  std::string last_read_data(
      reinterpret_cast<char*>(Cronet_Buffer_GetData(buffer)), bytes_read);
  Cronet_UrlRequest_Read(request, buffer);
}

void Response::OnSucceeded(Cronet_UrlRequestPtr request,
                           Cronet_UrlResponseInfoPtr info) {}

void Response::OnFailed(Cronet_UrlRequestPtr request,
                        Cronet_UrlResponseInfoPtr info, Cronet_ErrorPtr error) {
}

void Response::OnCanceled(Cronet_UrlRequestPtr request,
                          Cronet_UrlResponseInfoPtr info) {}

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
