#ifndef _SHELL_H
#define _SHELL_H

void shell(char *fdt);
void input(char *cmd);
void parse_cmd(char *cmd, char *argv);
void do_help();
void do_hello();
void do_mailbox();

#endif /* _SHELL_H */