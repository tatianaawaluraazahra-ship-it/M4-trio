#include <types.h>
#include <vmm.h>

extern void serial_print(const char* str);
extern void kernel_panic_at(const char* msg, const char* file, int line);

struct trap_frame {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

static void print_hex_pure(uint64_t val) {
    char buf[19];
    buf[0] = '0';
    buf[1] = 'x';
    const char *hex = "0123456789ABCDEF";
    for (int i = 15; i >= 0; i--) {
        buf[2 + i] = hex[val & 0xF];
        val >>= 4;
    }
    buf[18] = '\0';
    serial_print(buf);
}

static void log_field(const char *name, uint64_t val) {
    serial_print("  ");
    serial_print(name);
    serial_print(": ");
    print_hex_pure(val);
    serial_print("\n");
}

static void log_bool(const char *name, bool condition) {
    serial_print("  ");
    serial_print(name);
    serial_print(": ");
    if (condition) {
        serial_print("1 (TRUE)\n");
    } else {
        serial_print("0 (FALSE)\n");
    }
}

void page_fault_dump(uint64_t error_code, const struct trap_frame *tf) {
    uint64_t cr2 = vmm_read_cr2();
    
    serial_print("\n================ [M7 SYSTEM EXCEPTION] ================\n");
    serial_print("#PF Page Fault Diagnostics Log System Detected\n");
    
    log_field("CR2 Fault Address", cr2);
    log_field("Error Code Raw   ", error_code);
    log_field("Instruction RIP  ", tf->rip);
    log_field("Stack Pointer RSP", tf->rsp);
    
    log_bool("Present/Protection Violation (P)", (error_code & (1ULL << 0)) != 0);
    log_bool("Write Access Attempted       (W)", (error_code & (1ULL << 1)) != 0);
    log_bool("User Mode Execution Context  (U)", (error_code & (1ULL << 2)) != 0);
    log_bool("Reserved Bit Mismatch Fault  (RSVD)", (error_code & (1ULL << 3)) != 0);
    log_bool("Instruction Fetch Violation  (I/D)", (error_code & (1ULL << 4)) != 0);
    
    serial_print("=======================================================\n");
    kernel_panic_at("CRITICAL CRASH: Page Fault unhandled in M7 Kernel Space Loop.", __FILE__, __LINE__);
}
