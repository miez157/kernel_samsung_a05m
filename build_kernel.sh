#!/bin/bash

# === Setup ===
set -e
export ARCH=arm64
export ANDROID_MAJOR_VERSION=r

# === Cloning Azure Clang ===
CLANG_DIR="toolchain/clang"
if [ ! -d "$CLANG_DIR" ]; then
    echo "[+] Cloning Azure Clang..."
    git clone --depth=1 https://gitlab.com/Panchajanya1999/azure-clang.git $CLANG_DIR
else
    echo "[i] Azure Clang already exists. Skipping clone."
fi

# === Export Paths ===
echo "[+] Setting up environment variables..."
CLANG_PATH=$(pwd)/$CLANG_DIR/bin
export PATH=$CLANG_PATH:$PATH
export CROSS_COMPILE=${CLANG_PATH}/aarch64-linux-gnu-
export CC=${CLANG_PATH}/clang
export CLANG_TRIPLE=aarch64-linux-gnu-

# === Build Kernel ===
echo "[+] Starting kernel build..."
make -C $(pwd) O=$(pwd)/out KCFLAGS=-w CONFIG_SECTION_MISMATCH_WARN_ONLY=y LLVM=1 LLVM_IAS=1 a05m_defconfig
make -C $(pwd) O=$(pwd)/out KCFLAGS=-w CONFIG_SECTION_MISMATCH_WARN_ONLY=y LLVM=1 LLVM_IAS=1 -j$(nproc --all)

# === Copy Output ===
echo "[+] Copying built Image..."
cp out/arch/arm64/boot/Image $(pwd)/arch/arm64/boot/Image

echo "[✔] Build completed successfully."
