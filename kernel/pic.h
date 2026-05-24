#ifndef MCSOS_PIC_H
#define MCSOS_PIC_H

#include "types.h"

// Definisikan alamat port I/O untuk PIC Master dan Slave
#define PIC_MASTER_CMD      0x20
#define PIC_MASTER_DATA     0x21
#define PIC_SLAVE_CMD       0xA0
#define PIC_SLAVE_DATA      0xA1

// Perintah End-of-Interrupt (EOI)
#define PIC_EOI             0x20

// Deklarasi fungsi sesuai kontrak implementasi dosen
void pic_remap(uint8_t master_offset, uint8_t slave_offset);
void pic_mask_all(void);
void pic_unmask_irq(uint8_t irq);
void pic_send_eoi(uint8_t irq);

#endif