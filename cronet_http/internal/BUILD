load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//cronet_http:__subpackages__"])

cc_library(
    name = "memory",
    hdrs = ["memory.h"],
)

cc_library(
    name = "executor",
    hdrs = ["executor.h"],
    srcs = ["executor.cc"],
    deps = [
        "@prebuilt_cronet_native",
        "@abseil-cpp//absl/synchronization",
        "//cronet_http/internal:memory",
    ],
)
