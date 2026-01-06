#ifndef CRONET_HTTP_REQUEST_H_
#define CRONET_HTTP_REQUEST_H_

#include <string>
#include <string_view>

namespace cronet_http {

class Request {
 public:
  std::string url() const { return url_; }
  void set_url(std::string_view url) { url_ = url; }

 private:
  std::string url_;
};

}  // namespace cronet_http

#endif  // CRONET_HTTP_REQUEST_H_
