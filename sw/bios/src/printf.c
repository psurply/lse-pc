#include <bios.h>
#include <types.h>
#include <uart.h>

static const char digits[] = "0123456789abcdef";

# define INT_UNSIGNED	1
# define INT_SIGNED	2

# define BASE_HEX	16
# define BASE_OCT	8

# define BUF_SIZE	64

/*
 * some types
 */

struct s_file {
	char buff[BUF_SIZE + 1];
	unsigned int pos;
};

union u_arg
{
	unsigned long value;
	unsigned int uint;
	int sint;
	void *ptr;
};

typedef int (*t_fmtfun)(union u_arg *arg, struct s_file *file, int flags);

struct s_format
{
	char ch;
	t_fmtfun fun;
	int flags;
};

/*
 * buffered I/O
 */

static void fflush(struct s_file *file)
{
	file->buff[file->pos] = 0;
	uart1_write(file->buff);
	file->pos = 0;
}

static struct s_file* init_buffered_output(void)
{
	static struct s_file res;

	res.pos = 0;
	return &res;
}

static void putc(char c, struct s_file *file)
{
	file->buff[file->pos++] = c;
	if (file->pos == BUF_SIZE)
		fflush(file);
}

static void close_buffered_output(struct s_file *file)
{
	fflush(file);
}

static int conv(unsigned int n, int base, int dig[])
{
	int i = 0;

	while (n) {
		dig[i] = n % base;
		++i;
		n /= base;
	}

	return i - 1;
}

static int printnbr_base(unsigned int n,
		const char base[],
		int card,
		struct s_file *file)
{
	int digits[96];
	int i;
	int count;

	if (n == 0) {
		putc('0', file);
		return 1;
	}

	count = i = conv(n, card, digits);
	for (; i >= 0; --i)
		putc(base[digits[i]], file);
	return count;
}

static int print_int(union u_arg* value, struct s_file* file, int flags)
{
	int	sh = 0;

	if (value->sint == 0) {
		putc('0', file);
		return 1;
	}
	if (flags == INT_SIGNED) {
		if (value->sint < 0) {
			putc('-', file);
			value->uint = -value->sint;
			sh = 1;
		}
		else
			value->uint = value->sint;
	}
	return printnbr_base(value->uint, digits, 10, file) + sh;
}

static int print_str(union u_arg* value, struct s_file* file, int flags)
{
	int count = 0;
	char* s = value->ptr;

	flags = flags;
	for (; *s; ++count, ++s)
		putc(*s, file);
	return count;
}

static int print_char(union u_arg* value, struct s_file* file, int flags)
{
	char c;

	flags = flags;
	c = value->sint;
	putc(c, file);
	return 1;
}

static int print_base(union u_arg* value, struct s_file* file, int flags)
{
	return printnbr_base(value->uint, digits, flags, file);
}

static int print_ptr(union u_arg* value, struct s_file* file, int flags)
{
	int s = 0;
	union u_arg val;

	flags = flags;

	val.ptr = "0x";
	s += print_str(&val, file, 0);
	return s + printnbr_base(value->uint, digits, BASE_HEX, file);
}

static const struct s_format formats[] =
{
	{ 'd', print_int, INT_SIGNED },
	{ 'i', print_int, INT_SIGNED },
	{ 'u', print_int, INT_UNSIGNED },
	{ 's', print_str, 0 },
	{ 'c', print_char, 0 },
	{ 'o', print_base, BASE_OCT },
	{ 'x', print_base, BASE_HEX },
	{ 'p', print_ptr, 0 },
	{ 0, NULL, 0 }
};

static int special_char(char fmt, union u_arg* value, struct s_file* file)
{
	int i;

	for (i = 0; formats[i].fun; ++i)
		if (formats[i].ch == fmt)
			break;
	if (formats[i].fun)
		return formats[i].fun(value, file, formats[i].flags);
	else {
		if (fmt != '%')
			putc('%', file);
		putc(fmt, file);
		return 1 + (fmt != '%');
	}
}

int vprintf(const char* format, va_list args)
{
	struct s_file *file;
	union u_arg arg;
	int count = 0;

	for (file = init_buffered_output();
		*format;
		format += (*format == '%' ? 2 : 1)) {
		if (*format == '%') {
			arg.value = va_arg(args, unsigned long);
			count += special_char(*(format + 1), &arg, file);
		}
		else {
			putc(*format, file);
			++count;
		}
	}
	close_buffered_output(file);
	return count;
}

int printf(const char *format, ...)
{
	int res;
	va_list args;

	va_start(args, format);
	res = vprintf(format, args);
	va_end(args);
	return res;
}
