# === Step 1: Install prerequisites ===
sudo apt update
sudo apt install -y build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo curl nasm qemu qemu-kvm

# === Step 2: Set up install paths ===
export PREFIX="$HOME/opt/cross"
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"

# Add to shell profile permanently
if ! grep -q 'export PATH=.*/opt/cross/bin' ~/.bashrc; then
    echo '' >> ~/.bashrc
    echo '# Add i386-elf cross compiler to PATH' >> ~/.bashrc
    echo 'export PATH="$HOME/opt/cross/bin:$PATH"' >> ~/.bashrc
fi

# === Step 3: Create a source folder ===
mkdir -p "$HOME/src"
cd "$HOME/src"

# === Step 4: Build and install binutils ===
curl -LO http://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz
tar xf binutils-2.39.tar.gz
mkdir -p binutils-build
cd binutils-build
../binutils-2.39/configure --target=$TARGET --prefix=$PREFIX --disable-nls --disable-werror
make -j$(nproc)
make install

# === Step 5: Build and install GCC (C only) ===
cd "$HOME/src"
curl -LO https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz
tar xf gcc-12.2.0.tar.gz
mkdir -p gcc-build
cd gcc-build
../gcc-12.2.0/configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c --without-headers
make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)
make install-gcc
make install-target-libgcc

# === Step 6: Confirm it's working ===
i386-elf-gcc --version
