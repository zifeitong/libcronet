#include <bit>
#include <iostream>

#include "cronet_http/client.h"
#include "cronet_http/request.h"
#include "cronet_http/response.h"

using cronet_http::Client;
using cronet_http::Request;

int main() {
  Request req;
  req.set_url("https://example.com");

  Client client;
  auto resp = client.Send(req);

  if (resp) {
    std::cout << "HTTP Status Code: " << (*resp)->http_status_code()
              << std::endl;

    const std::byte* data;
    size_t bytes_read;

    std::string body;
    while ((*resp)->Read(&data, &bytes_read)) {
      body += std::string_view(std::bit_cast<char*>(data), bytes_read);
    }
    std::cout << body << std::endl;
  }
}
