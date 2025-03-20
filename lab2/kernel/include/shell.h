#ifndef _SHELL_H
#define _SHELL_H

void shell(char *fdt);
void input(char *cmd);
void parse_cmd(char *cmd, char *argv);
void do_help();
void do_hello();
void do_mailbox();
void do_ls();
void do_cat(char *argv);
void do_memAlloc();
void do_get_initramd(char *fdt);
void do_dtb(char *fdt);

#endif /* _SHELL_H */