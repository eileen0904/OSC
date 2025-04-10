int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int n);
int strlen(const char *s);
void strncpy(char *dest, const char *src, int start, int n);
void *simple_alloc(unsigned int size);
static char *int_to_str(char *buf, int num, int base, int is_signed);
int vsprintf(char *buf, const char *fmt, __builtin_va_list args);
int str_to_int(char *str);
