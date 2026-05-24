#ifndef MCSOS_IO_H
#define MCSOS_IO_H

#include "types.h"

// Menulis 1 byte data ke port hardware (Output Byte)
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" :: "a"(value), "Nd"(port) : "memory");
}

// Membaca 1 byte data dari port hardware (Input Byte)
static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

// Memberikan jeda waktu sejenak agar hardware tua/legacy sempat memproses data
static inline void io_wait(void) {
    outb(0x80, 0);
}

// Mematikan interupsi global (Clear Interrupt) agar aman saat konfigurasi
static inline void cpu_cli(void) {
    __asm__ volatile ("cli" ::: "memory");
}

// Menyalakan interupsi global (Set Interrupt) setelah infrastruktur siap
static inline void cpu_sti(void) {
    __asm__ volatile ("sti" ::: "memory");
}

// Menghentikan kerja CPU sementara (Halt) sampai ada interupsi baru (hemat daya)
static inline void cpu_hlt(void) {
    __asm__ volatile ("hlt" ::: "memory");
}

// Membaca Code Segment (CS) register saat ini
static inline uint16_t read_cs(void) {
    uint16_t cs;
    __asm__ volatile ("mov %%cs, %0" : "=r"(cs));
    return cs;
}

#endif