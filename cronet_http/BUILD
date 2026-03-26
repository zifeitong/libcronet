load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "cronet_http",
    srcs = [
        "client.cc",
        "request.cc",
        "response.cc",
        "upload_data_provider.cc",
    ],
    hdrs = [
        "client.h",
        "request.h",
        "response.h",
        "upload_data_provider.h",
    ],
    deps = [
        "//cronet_http/internal:executor",
        "//cronet_http/internal:memory",
        "@abseil-cpp//absl/memory",
        "@abseil-cpp//absl/synchronization",
        "@prebuilt_cronet_native",
    ],
)
