load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//cronet_http:__subpackages__"])

cc_library(
    name = "memory",
    hdrs = ["memory.h"],
)

cc_library(
    name = "executor",
    srcs = ["executor.cc"],
    hdrs = ["executor.h"],
    deps = [
        "//cronet_http/internal:memory",
        "@abseil-cpp//absl/synchronization",
        "@prebuilt_cronet_native",
    ],
)
