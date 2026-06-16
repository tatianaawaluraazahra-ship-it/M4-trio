#include "mcsfs1j_adapter.h"
#include <mcsos/block.h>

// Deklarasi fungsi dari Block Layer (M14)
extern int blk_read(void *dev, uint32_t lba, void *buf);
extern int blk_write(void *dev, uint32_t lba, const void *buf);

// Adaptor untuk membaca blok
int m16_read_block_adapter(void *ctx, uint32_t lba, void *buf) {
    return blk_read(ctx, lba, buf);
}

// Adaptor untuk menulis blok
int m16_write_block_adapter(void *ctx, uint32_t lba, const void *buf) {
    return blk_write(ctx, lba, buf);
}
