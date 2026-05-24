#include <stdint.h>

struct trap_frame {
    uint64_t r15; uint64_t r14; uint64_t r13; uint64_t r12;
    uint64_t r11; uint64_t r10; uint64_t r9;  uint64_t r8;
    uint64_t rbp; uint64_t rdi; uint64_t rsi; uint64_t rdx;
    uint64_t rcx; uint64_t rbx; uint64_t rax;
    
    uint64_t trap_no;     
    uint64_t error_code;  
    
    uint64_t rip; uint64_t cs; uint64_t rflags; uint64_t rsp; uint64_t ss;
};

extern void timer_on_irq0(void);
extern void pic_send_eoi(uint8_t irq);

void x86_64_trap_dispatch(struct trap_frame *tf) {
    // KELAS 1: Hardware IRQ dari PIC (Vector 32..47)
    if (tf->trap_no >= 32 && tf->trap_no <= 47) {
        if (tf->trap_no == 32) {
            timer_on_irq0(); 
        }
        pic_send_eoi(tf->trap_no - 32); 
    }
    // KELAS 2: Breakpoint Test (Vector 3) - Non-fatal
    else if (tf->trap_no == 3) {
        return;
    }
    // KELAS 3: Exception Lainnya - Fatal (Hentikan CPU langsung secara aman)
    else {
        while (1) {
            __asm__ volatile ("cli; hlt");
        }
    }
}
