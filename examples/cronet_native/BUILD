load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

cc_binary(
    name = "sample",
    srcs = [
        "main.cc",
        "sample_executor.cc",
        "sample_executor.h",
        "sample_url_request_callback.cc",
        "sample_url_request_callback.h",
    ],
    deps = ["@cronet_native"],
)
