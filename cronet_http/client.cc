#include "cronet_http/client.h"

#include <format>

#include "absl/memory/memory.h"
#include "cronet_http/internal/memory.h"
#include "response.h"

namespace cronet_http {

std::unique_ptr<Client> Client::Create() {
  auto client = absl::WrapUnique(new Client());

  MAKE_CRONET_C_UNIQUE_PTR(Cronet_EngineParams, engine_params);
  std::string version_string =
      Cronet_Engine_GetVersionString(client->engine_.get());
  Cronet_EngineParams_user_agent_set(
      engine_params.get(),
      std::format("cronet/{}.0.0.0",
                  version_string.substr(0, version_string.find('.')))
          .c_str());

  auto result =
      Cronet_Engine_StartWithParams(client->engine_.get(), engine_params.get());
  if (result != Cronet_RESULT_SUCCESS) {
    return nullptr;
  }

  return client;
}

Client::~Client() { Cronet_Engine_Shutdown(engine_.get()); }

auto Client::Do(const Request& request)
    -> std::expected<std::unique_ptr<Response>, Error> {
  // Build Cronet_UrlRequestParams.
  MAKE_CRONET_C_UNIQUE_PTR(Cronet_UrlRequestParams, request_params);
  Cronet_UrlRequestParams_http_method_set(request_params.get(),
                                          request.method().c_str());
  if (auto upload_data_provider = request.upload_data_provider()) {
    Cronet_UrlRequestParams_upload_data_provider_set(
        request_params.get(), upload_data_provider->GetPtr());
  }

  for (const auto& [name, value] : request.header()) {
    MAKE_CRONET_C_UNIQUE_PTR(Cronet_HttpHeader, header);
    Cronet_HttpHeader_name_set(header.get(), name.c_str());
    Cronet_HttpHeader_value_set(header.get(), value.c_str());
    Cronet_UrlRequestParams_request_headers_add(request_params.get(),
                                                header.get());
  }

  auto response = absl::WrapUnique(new Response());

  // Create and start Cronet_UrlRequest.
  MAKE_CRONET_C_UNIQUE_PTR(Cronet_UrlRequest, url_request);
  Cronet_UrlRequest_InitWithParams(
      url_request.get(), engine_.get(), request.url().c_str(),
      request_params.get(), response->callback_.get(), executor_.GetExecutor());
  Cronet_UrlRequest_Start(url_request.get());  // TODO: check return value

  // Transfer ownership of url_request to response.
  response->request_ = std::move(url_request);

  bool is_success = response->WaitUntilStarted();
  if (is_success) {
    return response;
  } else {
    return std::unexpected(Error{});
  }
}

}  // namespace cronet_http
