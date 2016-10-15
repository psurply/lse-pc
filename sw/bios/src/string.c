#include <bios.h>
#include <string.h>

u32 strlen(const char *str)
{
	const char *tmp = str;

	for (/* Nothing */; *tmp != '\0'; ++tmp)
		continue;

	return tmp - str;
}

int strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2++) {
		if (*s1++ == '\0')
			return 0;
	}

	return *(u8 *)s1 - *(u8 *)--s2;
}

int strncmp(const char *s1, const char *s2, u32 n)
{
	if (n == 0)
		return 0;

	do {
		if (*s1 != *s2++)
			return *(u8 *)s1 - *(u8 *)--s2;

		if (*s1++ == 0)
			break;
	} while (--n != 0);

	return 0;
}

void *memset(void *s, int c, u32 n)
{
	for (u32 i = 0; i < n; ++i)
		((char *)s)[i] = c;

	return s;
}

char *strtok_c(char *str, char delim)
{
	static char *last = NULL;

	char *run = str != NULL ? str : last;
	if (run == NULL)
		return NULL;

	/* Consume all starting delimiters */
	while (*run == delim)
		++run;

	char *res = run;

	while (*run != delim && *run != '\0')
		++run;

	if (*run != '\0') {
		char *end = run;

		/* Consume all ending delimiters */
		while (*run == delim)
			++run;

		*end = '\0';
	}

	last = *run == '\0' ? NULL : run;

	return res;
}

void reverse(char *str)
{
	for (int i = 0, j = strlen(str) - 1; i < j; ++i, --j) {
		char tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;
	}
}

int itoa(u32 n, u8 base, char *buffer)
{
	/* Cannot handle base > 16 */
	if (buffer == NULL || base > 16)
		return 1;

	u8 i = 0;

	if (n == 0) {
		buffer[i++] = '0';
	} else {
		for (/* Nothing */; n != 0; ++i, n /= base)
			buffer[i] = "0123456789ABCDEF"[n % base];

		buffer[i] = '\0';
		reverse(buffer);
	}

	return 0;
}

int atoi(const char *str, u32 *res)
{
	if (str == NULL)
		return 1;

	*res = 0;

	u8 i = 0;
	u8 base = 10;

	if (str[i] == '0') {
		++i;
		if (str[i] == 'x') {
			base = 16;
			++i;
		}
		else if (str[i] == 'b') {
			base = 2;
			++i;
		}
		else
			base = 8;
	}

	for (/* Nothing */; str[i] != '\0'; ++i) {
		u8 n = str[i];

		if (n >= 'a' && n <= 'f')
			n = n + 10 - 'a';
		else if (n >= 'A' && n <= 'F')
			n = n + 10 - 'A';
		else if (n >= '0' && n <= '9')
			n -= '0';
		else
			return 1;

		if (n >= base)
			return 1;

		*res = *res * base + n;
	}

	return 0;
}

int is_printable(char c)
{
	return c >= 0x20 && c <= 0x7E;
}

char *strncpy(char *dest, const char *src, size_t n)
{
	size_t i;

	for (i = 0; i < n && src[i] != '\0'; i++)
		dest[i] = src[i];
	for ( ; i < n; i++)
		dest[i] = '\0';

	return dest;
}
