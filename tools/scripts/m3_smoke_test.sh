#!/usr/bin/env bash
set -Eeuo pipefail

KERNEL_PANIC="build/kernel.panic.elf"
LOG_OUT="build/qemu.panic.log"

if [ ! -f "$KERNEL_PANIC" ]; then
    make panic
fi

rm -f "$LOG_OUT"

qemu-system-x86_64 \
    -no-reboot \
    -no-shutdown \
    -M q35 \
    -m 256M \
    -kernel "$KERNEL_PANIC" \
    -serial file:"$LOG_OUT" \
    -display none &
QEMU_PID=$!

sleep 2
kill -9 $QEMU_PID 2>/dev/null || true

if [ ! -f "$LOG_OUT" ]; then
    echo "FAIL: file log serial tidak terbentuk"
    exit 1
fi

echo "=== ISI LOG SERIAL ==="
cat "$LOG_OUT"
echo "======================"

if grep -q "MCSOS KERNEL PANIC" "$LOG_OUT" && grep -q "state=halted" "$LOG_OUT"; then
    echo "PASS: Smoke test M3 berhasil"
else
    echo "FAIL: Struktur panic tidak lengkap dalam log"
    exit 1
fi
