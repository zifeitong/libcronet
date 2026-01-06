#include <iostream>

#include "cronet_http/client.h"
#include "cronet_http/request.h"
#include "cronet_http/response.h"

using cronet_http::Client;
using cronet_http::Request;

int main() {
  Client client{};

  Request req;
  req.set_url("https://example.com");
  auto resp = client.Send(req);

  if (resp) {
    std::cout << resp->http_status_text() << std::endl;
  }
}
