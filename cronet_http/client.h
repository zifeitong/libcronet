#ifndef CRONET_HTTP_CLIENT_H_
#define CRONET_HTTP_CLIENT_H_

#include <expected>
#include <memory>

#include "cronet_http/internal/executor.h"
#include "cronet_http/internal/memory.h"
#include "cronet_http/request.h"
#include "cronet_http/response.h"

namespace cronet_http {

class Error {};

class Client {
 public:
  static std::unique_ptr<Client> Create();

  ~Client();

  auto Do(const Request& request)
      -> std::expected<std::unique_ptr<Response>, Error>;

 private:
  // Private constructor, use Create() factory method instead.
  Client() = default;

  MAKE_CRONET_C_UNIQUE_PTR(Cronet_Engine, engine_);
  internal::Executor executor_;
};

}  // namespace cronet_http

#endif  // CRONET_HTTP_CLIENT_H_
