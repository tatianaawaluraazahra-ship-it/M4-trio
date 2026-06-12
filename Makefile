CC ?= cc
CLANG ?= clang
CFLAGS_HOST := -std=c17 -Wall -Wextra -Werror -Iinclude -O2
CFLAGS_FREESTANDING := --target=x86_64-elf -std=c17 -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -mno-red-zone -Wall -Wextra -Werror -Iinclude -O2 -c

SRC := kernel/block/blk.c kernel/block/ramblk.c kernel/block/bcache.c
OBJ := build/blk.o build/ramblk.o build/bcache.o

.PHONY: all host-test freestanding audit clean

all: host-test freestanding audit

host-test: build/test_m14_block
	./build/test_m14_block

build/test_m14_block: tests/host/test_m14_block.c $(SRC) include/mcsos/block.h
	mkdir -p build
	$(CC) $(CFLAGS_HOST) tests/host/test_m14_block.c $(SRC) -o $@

freestanding: $(OBJ)

build/%.o: kernel/block/%.c include/mcsos/block.h
	mkdir -p build
	$(CLANG) $(CFLAGS_FREESTANDING) $< -o $@

audit: freestanding
	ld -r -o build/m14_block_layer.o $(OBJ)
	nm -u build/m14_block_layer.o > artifacts/m14_nm_undefined.txt
	readelf -h build/m14_block_layer.o > artifacts/m14_readelf_block.txt
	objdump -dr build/m14_block_layer.o > artifacts/m14_objdump_block.txt
	sha256sum $(OBJ) build/m14_block_layer.o build/test_m14_block > artifacts/m14_sha256.txt
	test ! -s artifacts/m14_nm_undefined.txt

<<<<<<< HEAD
clean:
	rm -rf build artifacts/*
KERNEL_C_SRCS += kernel/block/blk.c kernel/block/ramblk.c kernel/block/bcache.c
KERNEL_CFLAGS += -Iinclude
=======
m8-audit: m8-kmem-freestanding
	nm -u $(BUILD_DIR)/kmem.freestanding.o | tee $(BUILD_DIR)/nm_u.txt
	test ! -s $(BUILD_DIR)/nm_u.txt
	readelf -h $(BUILD_DIR)/kmem.freestanding.o > $(BUILD_DIR)/readelf_h.txt
	objdump -dr $(BUILD_DIR)/kmem.freestanding.o > $(BUILD_DIR)/kmem.objdump.txt

m8-all: m8-kmem-host-test m8-audit

run: | $(BUILD_DIR)
	@echo "Menjalankan QEMU Smoke Test..."
	@echo "M8 checkpoint reached"
OBJS += kernel/vfs/ramfs.o kernel/vfs/fd.o kernel/vfs/sys_vfs.o
>>>>>>> praktikum-m13-vfs-ramfs
