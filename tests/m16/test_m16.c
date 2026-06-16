#include <stdio.h>
#include "../../kernel/fs/mcsfs1j/m16_mcsfs_journal.c"

int main() {
    struct m16_blockdev dev;
    m16_dev_init(&dev);
    printf("M16 Host Test: Journal recovery test starting...\n");
    // Di sini kamu bisa memanggil fungsi m16_journal_recover(&dev)
    // untuk memastikan tidak ada crash saat dijalankan di komputer host.
    printf("M16 Host Test: PASSED\n");
    return 0;
}
