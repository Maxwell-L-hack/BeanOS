#!/usr/bin/env bash
set -euo pipefail

# 1. Install Dependencies
sudo dnf update -y
sudo dnf install -y \
  gcc gcc-c++ make bison flex gmp-devel mpfr-devel libmpc-devel texinfo \
  libstdc++-devel.i686 glibc-devel.i686 libgcc.i686 \
  binutils-gold curl

# 2. Set Target Variables
export PREFIX="${1:-/usr/local/i386elfgcc}"
export TARGET="i386-elf"
export PATH="$PREFIX/bin:$PATH"

# 3. Build Binutils
cd /tmp/src
rm -rf binutils-2.39 build-binutils
curl -LO https://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz
tar xf binutils-2.39.tar.gz
mkdir build-binutils && cd build-binutils
../binutils-2.39/configure \
  --target="$TARGET" \
  --prefix="$PREFIX" \
  --enable-interwork --enable-multilib \
  --disable-nls --disable-werror | tee configure-binutils.log
make -j$(nproc)
sudo make install

# 4. Build GCC
cd /tmp/src
rm -rf gcc-12.2.0 build-gcc
curl -LO https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz
tar xf gcc-12.2.0.tar.gz
mkdir build-gcc && cd build-gcc
../gcc-12.2.0/configure \
  --target="$TARGET" \
  --prefix="$PREFIX" \
  --disable-nls --disable-libssp \
  --enable-languages=c,c++ \
  --without-headers | tee configure-gcc.log
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
sudo make install-gcc
sudo make install-target-libgcc

# 5. Update PATH
if ! grep -q "$PREFIX/bin" ~/.bashrc; then
  echo "export PATH=\"$PREFIX/bin:\$PATH\"" >> ~/.bashrc
fi

echo "Cross-compiler installed! Run: source ~/.bashrc"
