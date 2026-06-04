# ──────────  Toolchain  ──────────
ASM  := nasm
CC   := i386-elf-gcc
LD   := i386-elf-ld
QEMU := qemu-system-i386

# ──────────  Paths  ──────────
SRC_DIR        := system
OBJ_DIR        := build/obj
ISO_DIR        := build/iso
FS_IMG         := build/fs.img
ISO_IMG        := build/MyraOS.iso
KERNEL_ELF     := build/kernel.elf
LINKER_SCRIPT  := linker.ld

# ──────────  Source discovery  ──────────
SRC_FILES  := $(filter-out $(shell find $(SRC_DIR)/libc/src/libc_user -name '*.c'), $(shell find $(SRC_DIR) -name '*.c'))
OBJ_FILES  := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(SRC_FILES:.c=.o))

INCLUDE_DIRS := $(filter-out $(SRC_DIR)/libc/include/libc_user, $(shell find $(SRC_DIR) -type d -name include))
INCLUDE_FLAGS := $(foreach d,$(INCLUDE_DIRS),-I"$d")

# All .asm sources
ASM_FILES      := $(shell find $(SRC_DIR) -name '*.asm')
ASM_OBJ_FILES  := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, \
                   $(ASM_FILES:.asm=.o))

ALL_OBJS := $(OBJ_FILES) $(ASM_OBJ_FILES)

# ──────────  Configurable Flags  ──────────
DEBUG ?= 0

# Base CFLAGS - will be modified by DEBUG setting
CFLAGS := -ffreestanding -m32 -std=gnu99 -Wall -Wextra -fno-omit-frame-pointer $(INCLUDE_FLAGS)

# ──────────  Default target  ──────────
all: run

# ──────────  Build rules  ──────────
# C - with dynamic DEBUG handling
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "[GCC] $<"
	@mkdir -p $(dir $@)
	@if [ "$(DEBUG)" = "1" ]; then \
		EXTRA_FLAGS="-Og -g -ggdb3 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-inline -fno-ipa-sra -fno-tree-sra -fvar-tracking-assignments"; \
	else \
		EXTRA_FLAGS="-O2 -DNDEBUG"; \
	fi; \
	$(CC) $(CFLAGS) $$EXTRA_FLAGS -c $< -o $@

# Assembly
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm
	@echo "[NASM] $<"
	@mkdir -p $(dir $@)
	@$(ASM) -f elf32 $< -o $@

$(FS_IMG):
	@echo "[FS] Creating ext2 disk image"
	@dd if=/dev/zero of=$(FS_IMG) bs=1M count=32
	@mkfs.ext2 $(FS_IMG)

# Kernel ELF
$(KERNEL_ELF): $(ALL_OBJS) $(LINKER_SCRIPT)
	@echo "[LD] Linking kernel ELF"
	@mkdir -p $(dir $@)
	@$(LD) -T $(LINKER_SCRIPT) -nostdlib -m elf_i386 \
	       -o $@ $(ALL_OBJS)

# ──────────  GRUB ISO  ──────────
GRUB_CFG := $(SRC_DIR)/boot/grub/grub.cfg

$(ISO_IMG): $(KERNEL_ELF) $(GRUB_CFG)
	@echo "[ISO] Creating GRUB ISO"
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_ELF) $(ISO_DIR)/boot/kernel.elf
	@cp $(GRUB_CFG)   $(ISO_DIR)/boot/grub/grub.cfg
	@grub2-mkrescue -o $(ISO_IMG) $(ISO_DIR) >/dev/null 2>&1

# ──────────  Run targets  ──────────
run: $(ISO_IMG) $(FS_IMG)
	@echo "[QEMU] Running MyraOS"
	@$(QEMU) -cdrom $(ISO_IMG) \
	        -drive file=$(FS_IMG),format=raw,if=ide,index=0 \
	        -m 1024M

debug:
	@DEBUG=1 $(MAKE) $(ISO_IMG) $(FS_IMG)
	@echo "[QEMU] Running MyraOS (GDB mode)"
	@$(QEMU) -s -S -cdrom $(ISO_IMG) \
	        -drive file=$(FS_IMG),format=raw,if=ide,index=0 \
	        -m 1024M

# ──────────  Clean  ──────────
clean:
	@echo "[CLEAN] Removing build artifacts"
	@rm -rf build

# ──────────  libc_user build  ──────────
LIBC_USER_SRC := $(shell find $(SRC_DIR)/libc/src/libc_user -name '*.c')
LIBC_USER_OBJ := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(LIBC_USER_SRC:.c=.o))
LIBC_USER_A   := $(SRC_DIR)/libc/src/libc_user/libc_user.a
LIBC_USER_INC := -I$(SRC_DIR)/libc/include/libc_user

$(LIBC_USER_OBJ): CFLAGS += $(LIBC_USER_INC)

$(LIBC_USER_A): $(LIBC_USER_OBJ)
	@echo "[AR] Creating libc_user.a"
	@$(AR) rcs $@ $^

$(OBJ_DIR)/libc/src/libc_user/%.o: $(SRC_DIR)/libc/src/libc_user/%.c
	@echo "[GCC] $<"
	@mkdir -p $(dir $@)
	@if [ "$(DEBUG)" = "1" ]; then \
		EXTRA_FLAGS="-Og -g -ggdb3 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-inline -fno-ipa-sra -fno-tree-sra -fvar-tracking-assignments"; \
	else \
		EXTRA_FLAGS="-O2 -DNDEBUG"; \
	fi; \
	$(CC) $(CFLAGS) $$EXTRA_FLAGS -c $< -o $@

libc: $(LIBC_USER_A)

# ──────────  release  ──────────
RELEASE_DIR := release/MyraOS-$(VERSION)

.PHONY: fs-ok release release-latest

fs-ok:
	@test -f $(FS_IMG) || (echo "missing $(FS_IMG). build/populate it first"; false)

release: $(ISO_IMG) fs-ok
	mkdir -p $(RELEASE_DIR)
	cp -a $(ISO_IMG) $(RELEASE_DIR)/MyraOS.iso
	cp -a $(FS_IMG) $(RELEASE_DIR)/fs.img
	(cd $(RELEASE_DIR) && \
	  (sha256sum MyraOS.iso 2>/dev/null || shasum -a 256 MyraOS.iso) > MyraOS.iso.sha256 && \
	  (sha256sum fs.img     2>/dev/null || shasum -a 256 fs.img)     > fs.img.sha256 && \
	  (sha256sum MyraOS.iso fs.img 2>/dev/null || shasum -a 256 MyraOS.iso fs.img) > CHECKSUMS.sha256)
