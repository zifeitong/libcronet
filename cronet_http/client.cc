#include "cronet_http/client.h"

#include "client.h"
#include "cronet_http/internal/memory.h"

namespace cronet_http {

Client::Client() {
  MAKE_CRONET_C_UNIQUE_PTR(Cronet_EngineParams, engine_params);
  Cronet_EngineParams_user_agent_set(engine_params.get(), "CronetSample/1");
  Cronet_EngineParams_enable_quic_set(engine_params.get(), true);

  Cronet_Engine_StartWithParams(engine_.get(), engine_params.get());
}

Client::~Client() { Cronet_Engine_Shutdown(engine_.get()); }

auto Client::Send(const Request& request) -> std::expected<Response, Error> {
  MAKE_CRONET_C_UNIQUE_PTR(Cronet_UrlRequestParams, request_params);
  Cronet_UrlRequestParams_http_method_set(request_params.get(), "GET");

  Response response;

  MAKE_CRONET_C_UNIQUE_PTR(Cronet_UrlRequest, url_request);
  Cronet_UrlRequest_InitWithParams(url_request.get(), engine_.get(),
                                   request.url().c_str(), request_params.get(),
                                   response.callback_, executor_.GetExecutor());

  Cronet_UrlRequest_Start(url_request.get());
  return response;
}

}  // namespace cronet_http
