#!/usr/bin/env bash
set -Eeuo pipefail
ISO="${1:-build/mcsos.iso}"
LOG="${2:-build/m3_serial.log}"
TIMEOUT_SEC="${MCSOS_QEMU_TIMEOUT:-8}"

OVMF_CODE="build/ovmf/OVMF_CODE_4M.fd"
OVMF_VARS="build/ovmf/OVMF_VARS_4M.fd"

fail() { echo "FAIL: $*" >&2; exit 1; }
test -f "$ISO" || fail "ISO tidak ditemukan: $ISO"
command -v qemu-system-x86_64 >/dev/null 2>&1 || fail "qemu-system-x86_64 tidak ditemukan"
test -f "$OVMF_CODE" || fail "OVMF_CODE tidak ditemukan: $OVMF_CODE"
test -f "$OVMF_VARS" || fail "OVMF_VARS tidak ditemukan: $OVMF_VARS"
mkdir -p "$(dirname "$LOG")"
rm -f "$LOG"

timeout "$TIMEOUT_SEC" qemu-system-x86_64 \
    -machine q35 \
    -m 256M \
    -smp 1 \
    -cpu qemu64 \
    -drive if=pflash,format=raw,readonly=on,file="$OVMF_CODE" \
    -drive if=pflash,format=raw,file="$OVMF_VARS" \
    -cdrom "$ISO" \
    -boot d \
    -serial file:"$LOG" \
    -display none \
    -no-reboot \
    -no-shutdown || true

if [ ! -f "$LOG" ] || [ ! -s "$LOG" ]; then
    fail "Log serial kosong atau tidak terbentuk"
fi

cat "$LOG"

if grep -q "MCSOS KERNEL PANIC" "$LOG"; then
    grep -q "reason=intentional M3 panic test" "$LOG" || fail "Log panic tidak sesuai kontrak"
    grep -q "state=halted" "$LOG" || fail "Kernel tidak masuk status halted setelah panic"
    echo "PASS: QEMU smoke test M3 selesai (Jalur Intentional Panic)"
else
    grep -q 'kernel entered' "$LOG" || fail "log boot M3 tidak ditemukan"
    grep -q 'selftest: basic invariants passed' "$LOG" || fail "selftest M3 tidak lulus"
    echo "PASS: QEMU smoke test M3 selesai (Jalur Normal)"
fi
