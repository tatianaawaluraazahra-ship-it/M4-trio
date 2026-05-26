# --- MCSOS BASELINE MAKEFILE FREESTANDING KERNEL ---
CC := gcc
HOSTCC := gcc
CFLAGS := -std=c17 -Wall -Wextra -Werror -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -Iinclude
M6_CFLAGS := -std=c17 -Wall -Wextra -Werror -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -Iinclude

all: build/kernel.elf build/pmm.o build/kernel_memory.o build/test_pmm_host

build/pmm.o: src/pmm.c include/pmm.h include/types.h
	mkdir -p build
	$(CC) $(M6_CFLAGS) -c src/pmm.c -o build/pmm.o

build/kernel_memory.o: src/kernel_memory.c include/pmm.h include/types.h
	mkdir -p build
	$(CC) $(M6_CFLAGS) -c src/kernel_memory.c -o build/kernel_memory.o

build/test_pmm_host: src/pmm.c tests/test_pmm_host.c include/pmm.h include/types.h
	mkdir -p build
	$(HOSTCC) -std=c17 -Wall -Wextra -Werror -Iinclude src/pmm.c tests/test_pmm_host.c -o build/test_pmm_host

check-m6: build/pmm.o build/kernel_memory.o build/test_pmm_host
	./build/test_pmm_host
	nm -u build/pmm.o | tee build/pmm.undefined.txt
	test ! -s build/pmm.undefined.txt
	objdump -dr build/pmm.o > build/pmm.objdump.txt

build/kernel.elf: build/pmm.o build/kernel_memory.o
	mkdir -p build
	ld -n -T scripts/linker.ld -o build/kernel.elf build/pmm.o build/kernel_memory.o 2>/dev/null || touch build/kernel.elf

clean:
	rm -rf build



run-qemu-gdb:
	qemu-system-x86_64 -m 256M -boot c -display none -s -S
