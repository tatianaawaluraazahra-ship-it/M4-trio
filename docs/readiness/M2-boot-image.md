# Readiness Review M2 - Boot Image dan Early Serial Console

## Identitas
- Proyek: MCSOS 260502
- Praktikum: M2
- Target: x86_64, QEMU, OVMF, Limine
- Nama/Kelompok: Tatiana Awalura Azahra, Rizwa Rahmatunnisa, Ai Fitri Sobariah
- Commit hash: 702d6345e263a3442b33b7f03582f9657aa1f746
- Tanggal: 2026-05-15

## Ringkasan Status
Status yang diajukan: siap uji QEMU tahap M2.
Alasan ringkas: Konfigurasi compiler Clang/LLD valid, kernel berhasil dibangun di alamat virtual higher-half (0xffffffff80000000), file ISO telah melewati verifikasi checksum, dan preflight check menunjukkan status OK.

## Evidence Matrix
| Evidence | Lokasi | Status | Catatan |
|---|---|---|---|
| Preflight M2 | `build/meta/m2-preflight.txt` | PASS | Semua dependensi dan jalur file valid. |
| Kernel ELF | `build/kernel.elf` | PASS | File eksekusi ELF64 berhasil dibuat. |
| Kernel map | `build/kernel.map` | PASS | Simbol memori terpetakan dengan benar. |
| readelf header | `build/inspect/readelf-header.txt` | PASS | Entry point valid di ffffffff80000000. |
| readelf PHDR | `build/inspect/readelf-program-headers.txt` | PASS | Program headers LOAD terdeteksi. |
| objdump | `build/inspect/objdump-disassembly.txt` | PASS | Instruksi assembly sesuai target x86_64. |
| ISO | `build/mcsos.iso` | PASS | Image bootable berhasil dibuat. |
| ISO checksum | `build/mcsos.iso.sha256` | PASS | Integritas file ISO terverifikasi. |
| Serial log | `build/qemu-serial.log` | PASS | Log memuat marker [SERIAL] M2. |
| Git commit | `build/meta/m2-commit.txt` | PASS | Snapshot kerja tersimpan di Git. |

## Invariants yang Diperiksa
1. Kernel adalah ELF64 x86_64.
2. Entry point sesuai linker script.
3. Kernel tidak memakai hosted libc.
4. Source dikompilasi dengan `-ffreestanding` dan `-mno-red-zone`.
5. Serial console tersedia sebelum subsistem kompleks.
6. Kernel tidak kembali setelah `kmain`.
7. Output QEMU disimpan sebagai log file.

## Failure Modes yang Diuji atau Dianalisis
| Failure mode | Pernah terjadi? | Diagnosis | Perbaikan |
|---|---|---|---|
| Toolchain salah | Tidak | Sistem menggunakan Clang/LLD 21.1.8 | N/A |
| OVMF tidak ditemukan | Tidak | Path OVMF terdeteksi di /usr/share/OVMF/ | N/A |
| Limine gagal fetch | Tidak | Fetch v11.x-binary berhasil via script. | N/A |
| ISO gagal dibuat | Tidak | Paket xorriso terpasang dan berfungsi. | N/A |
| QEMU log kosong | Tidak | Serial port dikalibrasi dengan benar. | N/A |
| Entry point salah | Tidak | Sesuai dengan spesifikasi higher-half. | N/A |
| Reboot loop | Tidak | Kernel masuk ke halt loop secara aman. | N/A |
| CRLF script | Tidak | File script menggunakan format LF Linux. | N/A |

## Keputusan Readiness
- [✓] Lulus M2: siap uji QEMU tahap M2.
- [ ] Belum lulus M2: perlu perbaikan.

## Catatan Reviewer

