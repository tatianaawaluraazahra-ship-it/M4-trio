#include <mcsos/block.h>
#include <stddef.h>

static mcsos_blk_device_t *g_blk_devices[MCSOS_BLK_MAX_DEVICES];
static uint32_t g_blk_count;

void mcsos_blk_registry_reset(void) { g_blk_count = 0; }
mcsos_blk_status_t mcsos_blk_register(mcsos_blk_device_t *dev) {
    if (g_blk_count >= MCSOS_BLK_MAX_DEVICES) return MCSOS_BLK_EFULL;
    g_blk_devices[g_blk_count++] = dev;
    return MCSOS_BLK_OK;
}
mcsos_blk_device_t *mcsos_blk_get(uint32_t index) {
    if (index >= g_blk_count) return NULL;
    return g_blk_devices[index];
}
uint32_t mcsos_blk_count(void) { return g_blk_count; }

static mcsos_blk_status_t mcsos_blk_validate_range(mcsos_blk_device_t *dev, uint64_t lba, uint32_t count, const void *buffer) {
    if (!dev || !buffer || count == 0u || !dev->ops) return MCSOS_BLK_EINVAL;
    if (lba >= dev->block_count || (uint64_t)count > dev->block_count - lba) return MCSOS_BLK_ERANGE;
    return MCSOS_BLK_OK;
}

mcsos_blk_status_t mcsos_blk_read(mcsos_blk_device_t *dev, uint64_t lba, uint32_t count, void *buffer) {
    mcsos_blk_status_t st = mcsos_blk_validate_range(dev, lba, count, buffer);
    if (st != MCSOS_BLK_OK) return st;
    return dev->ops->read(dev, lba, count, buffer);
}

mcsos_blk_status_t mcsos_blk_write(mcsos_blk_device_t *dev, uint64_t lba, uint32_t count, const void *buffer) {
    mcsos_blk_status_t st = mcsos_blk_validate_range(dev, lba, count, buffer);
    if (st != MCSOS_BLK_OK) return st;
    return dev->ops->write(dev, lba, count, (void *)buffer);
}

mcsos_blk_status_t mcsos_blk_flush(mcsos_blk_device_t *dev) {
    if (!dev || !dev->ops) return MCSOS_BLK_EINVAL;
    if (!dev->ops->flush) return MCSOS_BLK_OK;
    return dev->ops->flush(dev, 0, 0, 0);
}

void mcsos_blk_copy_name_for_driver(char dst[MCSOS_BLK_NAME_MAX], const char *src) {
    for (uint32_t i = 0; i < MCSOS_BLK_NAME_MAX - 1 && src[i]; i++) dst[i] = src[i];
}
