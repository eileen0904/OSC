#ifndef _SHELL_H
#define _SHELL_H

void shell(char *fdt);
void input(char *cmd);
char *parse_cmd(char *cmd);
void do_help();
void do_hello();
void do_mailbox();
void do_cat(char *argv);
void do_memAlloc();
void do_get_initramd(char *fdt);
void do_dtb(char *fdt);
void do_setTimeout(char *msg, char *sec);
void do_set2sAlert();

#endif /* _SHELL_H */