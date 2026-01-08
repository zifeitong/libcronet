#include "cronet_http/client.h"

#include "absl/memory/memory.h"
#include "cronet_http/internal/memory.h"
#include "response.h"

namespace cronet_http {

Client::Client() {
  MAKE_CRONET_C_UNIQUE_PTR(Cronet_EngineParams, engine_params);
  Cronet_EngineParams_user_agent_set(engine_params.get(), "CronetSample/1");
  Cronet_EngineParams_enable_quic_set(engine_params.get(), true);

  Cronet_Engine_StartWithParams(engine_.get(), engine_params.get());
}

Client::~Client() { Cronet_Engine_Shutdown(engine_.get()); }

auto Client::Send(const Request& request)
    -> std::expected<std::unique_ptr<Response>, Error> {
  // Build Cronet_UrlRequestParams.
  MAKE_CRONET_C_UNIQUE_PTR(Cronet_UrlRequestParams, request_params);
  Cronet_UrlRequestParams_http_method_set(request_params.get(), "GET");

  auto response = absl::WrapUnique(new Response());

  // Create and start Cronet_UrlRequest.
  MAKE_CRONET_C_UNIQUE_PTR(Cronet_UrlRequest, url_request);
  Cronet_UrlRequest_InitWithParams(
      url_request.get(), engine_.get(), request.url().c_str(),
      request_params.get(), response->callback_, executor_.GetExecutor());
  Cronet_UrlRequest_Start(url_request.get());

  // Transfer ownership of url_request to response.
  response->request_ = std::move(url_request);

  response->WaitUntilStarted();
  if (response->state_ != Response::State::kFailed) {
    return response;
  } else {
    return std::unexpected(Error{});
  }
}

}  // namespace cronet_http
