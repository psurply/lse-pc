#ifndef STRING_H_
# define STRING_H_

# include <types.h>

u32 strlen(const char *str);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, u32 n);
char *strncpy(char *dest, const char *src, size_t n);
void *memset(void *s, int c, u32 n);
char *strtok_c(char *str, char delim);
void reverse(char *str);

int itoa(u32 n, u8 base, char *buffer);
int atoi(const char *str, u32 *res);

int is_printable(char c);

#endif /* !STRING_H_ */
