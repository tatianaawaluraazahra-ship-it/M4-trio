#!/usr/bin/env bash
echo "--- Memulai Audit Baseline M0-M15 ---"

check_cmd() {
    if eval "$2" > /dev/null 2>&1; then
        echo "[OK] $1"
    else
        echo "[GAGAL] $1 (Periksa kembali modul terkait)"
    fi
}

check_cmd "M0: Struktur Repo" "test -d docs && test -d scripts"
check_cmd "M1: Toolchain" "clang --version && make --version"
check_cmd "M2: Boot Image" "test -d build"
check_cmd "M3: Logging" "grep -r \"panic\" kernel"
check_cmd "M4: IDT/Trap" "grep -r \"idt\|trap\" kernel"
check_cmd "M5: Timer/IRQ" "grep -r \"pit\|irq\|timer\" kernel"
check_cmd "M6: PMM Allocator" "grep -r \"pmm\" kernel"
check_cmd "M7: VMM Page Table" "grep -r \"vmm\|page\" kernel"
check_cmd "M8: Kernel Heap" "find . -name \"*heap*\" -o -name \"*malloc*\""
check_cmd "M9: Scheduler" "grep -r \"sched\|thread\" kernel"
check_cmd "M10: Syscall ABI" "grep -r \"syscall\" kernel"
check_cmd "M11: ELF Loader" "grep -r \"elf\" kernel"
check_cmd "M12: Locking" "grep -r \"spinlock\|mutex\" kernel"
check_cmd "M13: VFS" "grep -r \"vfs\|fd\" kernel"
check_cmd "M14: Block Device" "grep -r \"block\" kernel"
check_cmd "M15: MCSFS1" "find . -name \"*mcsfs*\""

echo "--- Audit Selesai ---"
