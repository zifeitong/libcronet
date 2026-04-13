#include <bit>
#include <iostream>

#include "cronet_http/client.h"
#include "cronet_http/request.h"
#include "cronet_http/response.h"
#include "cronet_http/upload_data_provider.h"

using cronet_http::Client;
using cronet_http::InMemoryUploadDataProvider;
using cronet_http::Request;
using cronet_http::Response;

void ReadAndPrintResponse(Response& resp) {
  std::cout << "HTTP Status Code: " << resp.http_status_code() << std::endl;

  const std::byte* data;
  size_t bytes_read;

  std::string body;
  while (resp.Read(&data, &bytes_read)) {
    body += std::string_view(std::bit_cast<const char*>(data), bytes_read);
  }
  std::cout << body << std::endl;
}

int main() {
  auto client = Client::Create();

  // Simple GET request.
  {
    Request req;
    req.set_method("GET");
    req.set_url("https://httpbin.org/get?foo=bar");

    auto resp = client->Do(req);

    if (resp) {
      ReadAndPrintResponse(*resp.value());
    }
  }

  // Simple POST request.
  {
    Request req;
    req.set_method("POST");
    req.set_url("https://httpbin.org/post");
    req.set_header({{"Header0", "Foo"}, {"Header1", "Bar"}});
    req.set_upload_data_provider(
        std::make_unique<InMemoryUploadDataProvider>("hello"));

    auto resp = client->Do(req);
    if (resp) {
      ReadAndPrintResponse(*resp.value());
    }
  }
}
