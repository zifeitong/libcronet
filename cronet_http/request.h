#ifndef CRONET_HTTP_REQUEST_H_
#define CRONET_HTTP_REQUEST_H_

#include <string>
#include <string_view>

#include "cronet_http/header.h"
#include "cronet_http/upload_data_provider.h"

namespace cronet_http {

class Request {
 public:
  const std::string& method() const { return method_; }
  void set_method(std::string method) { method_ = std::move(method); }

  const std::string& url() const { return url_; }
  void set_url(std::string url) { url_ = std::move(url); }

  const Header& header() const { return header_; }
  void set_header(Header header) { header_ = std::move(header); }

  const UploadDataProvider* upload_data_provider() const {
    return upload_data_provider_.get();
  }
  void set_upload_data_provider(
      std::unique_ptr<UploadDataProvider> upload_data_provider) {
    upload_data_provider_ = std::move(upload_data_provider);
  }

 private:
  std::string method_;
  std::string url_;
  Header header_;
  std::unique_ptr<UploadDataProvider> upload_data_provider_;
};

}  // namespace cronet_http

#endif  // CRONET_HTTP_REQUEST_H_
