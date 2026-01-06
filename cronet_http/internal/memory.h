#ifndef CRONET_HTTP_INTERNAL_UTIL_H_
#define CRONET_HTTP_INTERNAL_UTIL_H_

#include <memory>

namespace cronet_http::internal {

template <auto fn>
struct deleter {
  template <typename T>
  constexpr auto operator()(T* arg) const -> void {
    fn(arg);
  }
};

#define MAKE_CRONET_C_UNIQUE_PTR(T, var)                                \
  std::unique_ptr<T, cronet_http::internal::deleter<T##_Destroy>> var { \
    T##_Create()                                                        \
  }

}  // namespace cronet_http::internal

#endif  // CRONET_HTTP_INTERNAL_UTIL_H_
