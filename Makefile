CC ?= clang
HOST_CFLAGS := -std=c17 -Wall -Wextra -Werror -O2 -g
FREESTANDING_CFLAGS := -target x86_64-elf -std=c17 -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -mno-red-zone -Wall -Wextra -Werror -O2 -g

.PHONY: m15-all clean
m15-all: artifacts/m15/test_mcsfs1 artifacts/m15/mcsfs1.o artifacts/m15/mcsfs1.rel.o
	./artifacts/m15/test_mcsfs1 | tee artifacts/m15/host_test.txt
	nm -u artifacts/m15/mcsfs1.rel.o | tee artifacts/m15/nm_undefined.txt
	test ! -s artifacts/m15/nm_undefined.txt
	readelf -h artifacts/m15/mcsfs1.rel.o | tee artifacts/m15/readelf_header.txt
	objdump -dr artifacts/m15/mcsfs1.rel.o | tee artifacts/m15/objdump.txt >/dev/null
	sha256sum artifacts/m15/* | tee artifacts/m15/SHA256SUMS.txt

artifacts/m15/test_mcsfs1: tests/m15/test_mcsfs1.c fs/mcsfs1/mcsfs1.c fs/mcsfs1/mcsfs1.h
	mkdir -p artifacts/m15
	$(CC) $(HOST_CFLAGS) -I. tests/m15/test_mcsfs1.c fs/mcsfs1/mcsfs1.c -o $@

artifacts/m15/mcsfs1.o: fs/mcsfs1/mcsfs1.c fs/mcsfs1/mcsfs1.h
	mkdir -p artifacts/m15
	$(CC) $(FREESTANDING_CFLAGS) -I. -c fs/mcsfs1/mcsfs1.c -o $@

artifacts/m15/mcsfs1.rel.o: artifacts/m15/mcsfs1.o
	ld -r $< -o $@

clean:
	rm -rf artifacts/m15

# --- Integrasi M16 ---
M16_SRC := kernel/fs/mcsfs1j/m16_mcsfs_journal.c kernel/fs/mcsfs1j/mcsfs1j_adapter.c
M16_OBJ := $(M16_SRC:.c=.o)

# Aturan untuk membuat file .o dari .c
kernel/fs/mcsfs1j/%.o: kernel/fs/mcsfs1j/%.c
	$(CC) $(FREESTANDING_CFLAGS) -I. -Iinclude -Ikernel/include -c $< -o $@
# Tambahkan m16_all ke target utama
m16-all: $(M16_OBJ)
