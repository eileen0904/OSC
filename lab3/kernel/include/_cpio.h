#ifndef _CPIO_H_
#define _CPIO_H_

#define CPIO_NEWC_MAGIC "070701"

struct cpio_newc_header {
    char c_magic[6]; // determine whether this archive is written with little-endian or big-endian integers
    char c_ino[8];
    char c_mode[8];
    char c_uid[8];
    char c_gid[8];
    char c_nlink[8];
    char c_mtime[8];
    char c_filesize[8];
    char c_devmajor[8];
    char c_devminor[8];
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];
    char c_check[8];
};

struct c_file {
    char datas[100000];
    char names[10000];
    int names_pos[1024];
    int datas_pos[1024];
    int n;
};

unsigned int hex_to_int(const char *hex, int len);
void cpio_parse_file();
void cpio_ls();
void cpio_cat(char *file);
void cpio_load(char *str);

#endif /* _CPIO_H_ */