#ifndef _FDT_H
#define _FDT_H

#include <stdint.h>

/* stored in big-endian format */
struct fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};

struct fdt_node_header {
    uint32_t tag;
    char name[0];
};

struct fdt_property {
    uint32_t tag;
    uint32_t len;
    uint32_t nameoff;
    char data[0];
};

#define FDT_MAGIC 0xd00dfeed /* 4: version, 4: total size */
#define FDT_BEGIN_NODE 0x1 /* Start node: full name */
#define FDT_END_NODE 0x2   /* End node */
#define FDT_PROP 0x3       /* Property: name off, size, content */
#define FDT_NOP 0x4        /* nop */
#define FDT_END 0x9

#define fdt_get_header(fdt, field) (fdt32_ld(&((const struct fdt_header *)(fdt))->field))
#define fdt_magic(fdt) (fdt_get_header(fdt, magic))
#define fdt_totalsize(fdt) (fdt_get_header(fdt, totalsize))
#define fdt_off_dt_struct(fdt) (fdt_get_header(fdt, off_dt_struct))
#define fdt_off_dt_strings(fdt) (fdt_get_header(fdt, off_dt_strings))
#define fdt_version(fdt) (fdt_get_header(fdt, version))
#define fdt_last_comp_version(fdt) (fdt_get_header(fdt, last_comp_version))
#define fdt_size_dt_strings(fdt) (fdt_get_header(fdt, size_dt_strings))
#define fdt_size_dt_struct(fdt) (fdt_get_header(fdt, size_dt_struct))

#define fdtn_get_header(fdtn, field) \
    (fdt32_ld(&((const struct fdt_node_header *)(fdtn))->field))
#define fdtn_tag(fdtn) (fdtn_get_header(fdtn, tag))

#define fdtp_get_header(fdtp, field) \
    (fdt32_ld(&((const struct fdt_property *)(fdtp))->field))
#define fdtp_tag(fdtp) (fdtp_get_header(fdtp, tag))
#define fdtp_len(fdtp) (fdtp_get_header(fdtp, len))
#define fdtp_nameoff(fdtp) (fdtp_get_header(fdtp, nameoff))

static inline uint32_t fdt32_ld(const void *p) {
    return be32_to_cpu(*(const uint32_t *)p);
}

typedef uint32_t (*fdt_callback)(int type, char *name, char *data, uint32_t size);

uint32_t print_dtb(int type, char *name, char *data, uint32_t size);
uint32_t get_initramfs_addr(int type, char *name, char *data, uint32_t size);
uint32_t fdt_traverse(fdt_callback cb, char *dtb_ptr);

#endif /* _FDT_H */