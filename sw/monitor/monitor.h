#ifndef MONITOR_H
#define MONITOR_H

#include <lsepc.h>

struct monitor
{
	struct lsepc *lsepc;
	struct dev *dev;
};

int monitor_init(struct monitor *m, const char *dev);
void monitor_exec(struct monitor *m, const char *cmd);
void monitor_delete(struct monitor *m);

#endif /* MONITOR_H */
