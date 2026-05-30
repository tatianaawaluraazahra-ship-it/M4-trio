#ifndef MCSOS_BLOCK_BLK_H
#define MCSOS_BLOCK_BLK_H
#include <stddef.h>
#include <stdint.h>
#define BLOCK_SIZE 512
typedef struct block_device {
    uint32_t id;
    size_t size;
    int (*read)(struct block_device *dev, uint64_t block_id, void *buffer);
    int (*write)(struct block_device *dev, uint64_t block_id, const void *buffer);
    struct block_device *next;
} block_device_t;
void blk_register_device(block_device_t *dev);
block_device_t *blk_get_device(uint32_t id);
#endif
