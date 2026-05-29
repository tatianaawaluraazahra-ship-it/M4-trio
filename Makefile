CC ?= clang
CFLAGS_COMMON := -std=c17 -Wall -Wextra -Werror -Iinclude
CFLAGS_KERNEL := $(CFLAGS_COMMON) -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone
BUILD_DIR := build/m8

.PHONY: all check clean m8-clean m8-kmem-host-test m8-kmem-freestanding m8-audit m8-all run

all: m8-kmem-host-test

check: m8-kmem-host-test

clean: m8-clean
	rm -rf build

m8-clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

m8-kmem-freestanding: | $(BUILD_DIR)
	$(CC) $(CFLAGS_KERNEL) -c kernel/mm/kmem.c -o $(BUILD_DIR)/kmem.freestanding.o

m8-kmem-host-test: | $(BUILD_DIR)
	$(CC) $(CFLAGS_COMMON) -DMCSOS_HOST_TEST tests/test_kmem.c kernel/mm/kmem.c -o $(BUILD_DIR)/test_kmem
	./$(BUILD_DIR)/test_kmem | tee $(BUILD_DIR)/test_kmem.log

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
