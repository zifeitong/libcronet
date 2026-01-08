load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "cronet_http",
    srcs = [
        "client.cc",
        "request.cc",
        "response.cc",
        "sample_executor.cc",
        "sample_executor.h",
    ],
    hdrs = [
        "client.h",
        "request.h",
        "response.h",
    ],
    deps = [
        "//cronet_http/internal:memory",
        "@abseil-cpp//absl/memory",
        "@abseil-cpp//absl/synchronization",
        "@prebuilt_cronet_native",
    ],
)
