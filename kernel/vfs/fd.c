#include <mcsos/vfs/vfs.h>
#include <stddef.h>

void mcs_fd_table_init(mcs_fd_table_t *table) {
    for (size_t i = 0; i < (size_t)MCS_MAX_OPEN_FILES; i++) table->files[i].used = 0u;
}

static mcs_file_t *mcs_fd_get(mcs_fd_table_t *table, int fd) {
    if (fd < 0 || (size_t)fd >= (size_t)MCS_MAX_OPEN_FILES || !table->files[fd].used) return NULL;
    return &table->files[fd];
}

static int mcs_fd_alloc(mcs_fd_table_t *table) {
    for (size_t i = 0; i < (size_t)MCS_MAX_OPEN_FILES; i++) {
        if (!table->files[i].used) { table->files[i].used = 1u; return (int)i; }
    }
    return MCS_ENFILE;
}

int mcs_sys_open(mcs_process_t *proc, mcs_ramfs_t *fs, const char *path, uint32_t flags) {
    // Tambahkan validasi jalur absolut
    if (!path || path[0] != '/') return MCS_EINVAL;

    mcs_vnode_t *node;
    int fd = mcs_fd_alloc(&proc->fd_table);
    if (fd < 0) return fd;
    
    if (mcs_ramfs_lookup(fs, path, &node) != MCS_OK) {
        if (!(flags & MCS_O_CREAT) || mcs_ramfs_create_file(fs, path, &node) != MCS_OK) {
            proc->fd_table.files[fd].used = 0;
            return MCS_ENOENT;
        }
    }
    proc->fd_table.files[fd].flags = flags;
    proc->fd_table.files[fd].node = node;
    proc->fd_table.files[fd].fs = fs;
    proc->fd_table.files[fd].offset = 0;
    return fd;
}

mcs_ssize_t mcs_sys_read(mcs_process_t *proc, int fd, void *buf, size_t len) {
    mcs_file_t *f = mcs_fd_get(&proc->fd_table, fd);
    if (!f) return MCS_EBADF;
    size_t avail = f->node->size - f->offset;
    size_t n = (len < avail) ? len : avail;
    uint8_t *d = (uint8_t *)buf;
    for (size_t i = 0; i < n; i++) d[i] = f->fs->data[f->node->data_offset + f->offset + i];
    f->offset += n;
    return (mcs_ssize_t)n;
}

mcs_ssize_t mcs_sys_write(mcs_process_t *proc, int fd, const void *buf, size_t len) {
    mcs_file_t *f = mcs_fd_get(&proc->fd_table, fd);
    if (!f) return MCS_EBADF;
    const uint8_t *s = (const uint8_t *)buf;
    for (size_t i = 0; i < len; i++) f->fs->data[f->node->data_offset + f->offset + i] = s[i];
    f->offset += len;
    if (f->offset > f->node->size) f->node->size = f->offset;
    return (mcs_ssize_t)len;
}

int mcs_sys_close(mcs_process_t *proc, int fd) {
    mcs_file_t *f = mcs_fd_get(&proc->fd_table, fd);
    if (!f) return MCS_EBADF;
    f->used = 0u;
    return MCS_OK;
}

int mcs_sys_lseek(mcs_process_t *proc, int fd, long offset, int whence) {
    mcs_file_t *f = mcs_fd_get(&proc->fd_table, fd);
    if (!f) return MCS_EBADF;
    if (whence == MCS_SEEK_SET) f->offset = (size_t)offset;
    return (int)f->offset;
}
