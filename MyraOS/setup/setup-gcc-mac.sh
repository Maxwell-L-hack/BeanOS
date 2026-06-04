# === Step 1: Install prerequisites with Homebrew ===
brew install gmp mpfr libmpc texinfo nasm qemu curl

# === Step 2: Set up install paths ===
export PREFIX="$HOME/opt/cross"
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"

# Add to ~/.zprofile (for Zsh) if not already there
if ! grep -q "$PREFIX/bin" ~/.zprofile 2>/dev/null; then
    echo '' >> ~/.zprofile
    echo '# Add i386-elf cross compiler to PATH' >> ~/.zprofile
    echo 'export PATH="$HOME/opt/cross/bin:$PATH"' >> ~/.zprofile
fi

# === Step 3: Create source folder ===
mkdir -p "$HOME/src"
cd "$HOME/src"

# === Step 4: Build and install binutils ===
curl -LO http://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz
tar xf binutils-2.39.tar.gz
mkdir -p binutils-build
cd binutils-build
../binutils-2.39/configure --target=$TARGET --prefix=$PREFIX --disable-nls --disable-werror
make -j$(sysctl -n hw.ncpu)
make install

# === Step 5: Build and install GCC (C only) ===
cd "$HOME/src"
curl -LO https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz
tar xf gcc-12.2.0.tar.gz
mkdir -p gcc-build
cd gcc-build
../gcc-12.2.0/configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c --without-headers
make all-gcc -j$(sysctl -n hw.ncpu)
make all-target-libgcc -j$(sysctl -n hw.ncpu)
make install-gcc
make install-target-libgcc

# === Step 6: Confirm it's working ===
i386-elf-gcc --version
