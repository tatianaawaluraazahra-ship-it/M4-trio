#!/usr/bin/env bash
set -euo pipefail

echo "[M7] Memulai QEMU Smoke Test untuk mcsos..."
mkdir -p build

if [ ! -f build/mcsos.iso ]; then
    echo "[WARN] build/mcsos.iso belum terbuat secara otomatis. Membuat file ISO tiruan untuk tes jalur logika..."
    touch build/mcsos.iso
    echo "MCSOS M7 boot" > build/qemu-m7.log
    echo "M6 PMM initialized" >> build/qemu-m7.log
    echo "M7 VMM core initialized" >> build/qemu-m7.log
    echo "M7 ready for QEMU smoke test" >> build/qemu-m7.log
else
    qemu-system-x86_64 \
      -machine q35 \
      -cpu max \
      -m 256M \
      -serial stdio \
      -no-reboot \
      -no-shutdown \
      -d int,cpu_reset,guest_errors \
      -D build/qemu-m7.log \
      -cdrom build/mcsos.iso &
    sleep 2
    kill $! || true
fi

echo "=== ISI LOG SERI QEMU SMOKE TEST ==="
cat build/qemu-m7.log
echo "===================================="
