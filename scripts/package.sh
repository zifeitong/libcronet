#!/bin/bash
set -ex

pkgver=$1

pushd chromium-$pkgver/out/Release/cronet

# Define bazel module
cat > MODULE.bazel <<EOF
module(name = "cronet_native", version = "${pkgver}")
EOF

cat > BUILD.bazel <<EOF
cc_import(
  name = "cronet_native",
  hdrs = [
    "cronet_c.h",
    "cronet_export.h",
    "cronet.idl_c.h",
    "bidirectional_stream_c.h",
  ],
  shared_library = "libcronet.${pkgver}.so",
)
EOF

popd

tar -czf cronet_native-$pkgver.tar.gz -C chromium-$pkgver/out/Release/cronet .
