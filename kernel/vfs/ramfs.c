#include <mcsos/vfs/vfs.h>
#include <stddef.h>

void mcs_ramfs_init(mcs_ramfs_t *fs) {
    if (!fs) return;
    fs->node_count = 1u;
    fs->data_used = 0u;
    fs->nodes[0].used = 1u;
    fs->nodes[0].id = 0u;
    fs->nodes[0].type = MCS_VNODE_DIR;
    fs->nodes[0].name[0] = '/';
    fs->nodes[0].name[1] = '\0';
    fs->nodes[0].size = 0u;
}

int mcs_ramfs_lookup(mcs_ramfs_t *fs, const char *path, mcs_vnode_t **out_node) {
    if (!fs || !path || !out_node) return MCS_EINVAL;
    if (path[0] == '/' && path[1] == '\0') { *out_node = &fs->nodes[0]; return MCS_OK; }
    
    for (size_t i = 1; i < fs->node_count; i++) {
        const char *name = (path[0] == '/') ? path + 1 : path;
        size_t j = 0;
        while (name[j] == fs->nodes[i].name[j] && name[j] != '\0') j++;
        if (name[j] == '\0' && fs->nodes[i].name[j] == '\0') {
            *out_node = &fs->nodes[i];
            return MCS_OK;
        }
    }
    return MCS_ENOENT;
}

int mcs_ramfs_create_file(mcs_ramfs_t *fs, const char *path, mcs_vnode_t **out_node) {
    if (fs->node_count >= MCS_MAX_NODES) return MCS_ENOSPC;
    mcs_vnode_t *node = &fs->nodes[fs->node_count++];
    node->used = 1u;
    node->id = (uint32_t)(fs->node_count - 1);
    node->type = MCS_VNODE_FILE;
    node->data_capacity = 256;
    node->data_offset = fs->data_used;
    node->size = 0u;
    fs->data_used += 256;
    
    const char *name = (path[0] == '/') ? path + 1 : path;
    for (size_t i = 0; i < MCS_MAX_NAME - 1; i++) {
        node->name[i] = name[i];
        if (name[i] == '\0') break;
    }
    *out_node = node;
    return MCS_OK;
}

int mcs_ramfs_seed_file(mcs_ramfs_t *fs, const char *path, const uint8_t *data, size_t len) {
    mcs_vnode_t *node;
    int rc = mcs_ramfs_create_file(fs, path, &node);
    if (rc != MCS_OK) return rc;
    for (size_t i = 0; i < len; i++) fs->data[node->data_offset + i] = data[i];
    node->size = len;
    return MCS_OK;
}
