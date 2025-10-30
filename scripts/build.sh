#!/bin/bash

# Build cronet library.
#
# Usage: ./build.sh VERSION
#
# Reference:
#   https://gitlab.archlinux.org/archlinux/packaging/packages/chromium
#   https://gitweb.gentoo.org/repo/gentoo.git/tree/www-client/chromium
set -ex

pkgver=$1

if [ ! -d chromium-$pkgver ]; then
  echo "Download and extract chromium tarball..."
  wget -c --no-verbose --show-progress --progress=dot:mega https://commondatastorage.googleapis.com/chromium-browser-official/chromium-$pkgver-lite.tar.xz
  tar -xf chromium-$pkgver-lite.tar.xz
fi

pushd chromium-$pkgver

# Apply patches
ls ../patches/*.patch | xargs -n 1 patch -p1 -i

# Setup build environment
./tools/clang/scripts/update.py
./tools/rust/update_rust.py
./build/install-build-deps.sh
./build/linux/sysroot_scripts/install-sysroot.py --arch=x64
ln -s google.star ./build/config/siso/backend_config/backend.star

# Build shared library
gn_args=(
  'is_official_build=true'
  'disable_fieldtrial_testing_config=true'
  'symbol_level=0'
  'use_nss_certs=false'

  # headless build
  'enable_print_preview=false'
  'enable_remoting=false'
  'ozone_platform="headless"'
  'rtc_use_pipewire=false'
  'use_alsa=false'
  'use_cups=false'
  'use_gio=false'
  'use_glib=false'
  'use_gtk=false'
  'use_kerberos=false'
  'use_libpci=false'
  'use_pangocairo=false'
  'use_pulseaudio=false'
  'use_qt5=false'
  'use_qt6=false'
  'use_udev=false'
  'use_vaapi=false'
  'use_xkbcommon=false'
  'icu_use_data_file=false'
)

buildtools/linux64/gn gen out/Release --args="${gn_args[*]}"
third_party/siso/cipd/siso ninja --offline -C out/Release cronet_package
