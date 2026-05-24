#ifndef MCSOS_PIT_H
#define MCSOS_PIT_H

#include <stdint.h>

// Definisikan alamat port I/O untuk PIT
#define PIT_CHANNEL0_DATA   0x40
#define PIT_COMMAND         0x43

// Kontrak implementasi sesuai instruksi dosen
void pit_configure_hz(uint32_t hz);
void timer_on_irq0(void);

// Variabel counter detak global (wajib volatile karena berubah di jalur interrupt)
extern volatile uint64_t g_ticks;

#endif
