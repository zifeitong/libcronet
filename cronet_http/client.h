#ifndef CRONET_HTTP_CLIENT_H_
#define CRONET_HTTP_CLIENT_H_

#include <expected>
#include <memory>
#include <stop_token>

#include "cronet_c.h"
#include "cronet_http/internal/memory.h"
#include "cronet_http/request.h"
#include "cronet_http/response.h"
#include "cronet_http/sample_executor.h"

namespace cronet_http {

class Error {};

class Client {
 public:
  Client();
  ~Client();

  auto Send(const Request& request)
      -> std::expected<std::unique_ptr<Response>, Error>;

 private:
  MAKE_CRONET_C_UNIQUE_PTR(Cronet_Engine, engine_);
  SampleExecutor executor_;
};

}  // namespace cronet_http

#endif  // CRONET_HTTP_CLIENT_H_
