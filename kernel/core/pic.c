#include <pic.h>
#include <io.h>

void pic_remap(uint8_t master_offset, uint8_t slave_offset) {
    uint8_t mask_master = inb(PIC_MASTER_DATA);
    uint8_t mask_slave = inb(PIC_SLAVE_DATA);

    outb(PIC_MASTER_CMD, 0x11);
    io_wait();
    outb(PIC_SLAVE_CMD, 0x11);
    io_wait();

    outb(PIC_MASTER_DATA, master_offset);
    io_wait();
    outb(PIC_SLAVE_DATA, slave_offset);
    io_wait();

    outb(PIC_MASTER_DATA, 0x04);
    io_wait();
    outb(PIC_SLAVE_DATA, 0x02);
    io_wait();

    outb(PIC_MASTER_DATA, 0x01);
    io_wait();
    outb(PIC_SLAVE_DATA, 0x01);
    io_wait();

    outb(PIC_MASTER_DATA, mask_master);
    outb(PIC_SLAVE_DATA, mask_slave);
}

void pic_mask_all(void) {
    outb(PIC_MASTER_DATA, 0xFF);
    outb(PIC_SLAVE_DATA, 0xFF);
}

void pic_unmask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC_MASTER_DATA;
    } else {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC_SLAVE_CMD, PIC_EOI);
    }
    outb(PIC_MASTER_CMD, PIC_EOI);
}
