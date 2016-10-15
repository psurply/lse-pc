#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <lsepc.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define PROTO_CMD_SIZE		32

#define CMD_GETVERSION		1
#define CMD_ECHO		2
#define CMD_START		3
#define CMD_STOP		4
#define CMD_BRK			5
#define CMD_CONT		6
#define CMD_GETSTATUS		7
#define CMD_GETREGS		8
#define CMD_SETIODATA		9
#define CMD_GETIODATA		10
#define CMD_GETIOADDR		11

#define PROTO_SOH		0x01
#define PROTO_STX		0x02
#define PROTO_ETX		0x03
#define PROTO_EOT		0x04
#define PROTO_ENQ		0x05
#define PROTO_ACK		0x06
#define PROTO_NAK		0x15
#define PROTO_SYN		0x16

#define BAUDRATE		B115200

#define SYNC_FAILS		3

struct lsepc
{
	const char *dev_name;
	int dev;

	fd_set read_fds;
	fd_set write_fds;
	fd_set except_fds;

	struct termios term_cur;
	struct termios term_org;
};

struct lsepc_cmd_hdr
{
	uint8_t enq;
	uint8_t soh;
	uint8_t cmd;
	uint8_t stx;
} __attribute__((packed));

struct lsepc_cmd_ftr
{
	uint8_t eot;
} __attribute__((packed));

struct lsepc_resp_hdr
{
	uint8_t ack;
	uint8_t size;
	uint8_t stx;
} __attribute__((packed));

struct lsepc_resp_ftr
{
	uint8_t eot;
} __attribute__((packed));

struct lsepc_synack
{
	uint8_t syn;
	uint8_t ack;
} __attribute__((packed));

static int lsepc_read(int fd, void *buff, int size)
{
	int count = 0;
	int ret = 0;
	char *b = buff;
	fd_set read_fds, write_fds, except_fds;
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_ZERO(&except_fds);
	FD_SET(fd, &read_fds);

	do {
		if ((ret = select(fd + 1, &read_fds, &write_fds,
				&except_fds, &timeout)) == 1)
		{
			if ((ret = read(fd, b + count, size - count)) < 0)
				return errno;
		}
		else
		    return -ETIME;

		count += ret;
	} while (count < size);

	return 0;
}

static int lsepc_sync(struct lsepc *lsepc)
{
	uint8_t syn = PROTO_SYN;
	struct lsepc_synack synack;
	int ret;

	if (write(lsepc->dev, &syn, 1) != 1)
		return -ECOMM;

	if ((ret = lsepc_read(lsepc->dev, &synack,
			sizeof(struct lsepc_synack))) != 0)
		return ret;

	if (synack.syn != PROTO_SYN || synack.ack != PROTO_ACK)
		return -EBADMSG;

	return 0;
}

static int lsepc_getsync(struct lsepc *lsepc)
{
	for (int i = 0; i < SYNC_FAILS; ++i)
	{
		if (!lsepc_sync(lsepc))
			return 0;
	}

	return -EREMOTEIO;
}

static int lsepc_send_cmd(struct lsepc *lsepc, uint8_t cmd,
	const uint8_t *args, int size)
{
	char buff[PROTO_CMD_SIZE];
	struct lsepc_cmd_hdr *hdr = (void *) buff;
	struct lsepc_cmd_ftr *ftr = (void *)((char *) buff
		+ sizeof(struct lsepc_cmd_hdr) + size);
	int buff_size = sizeof(struct lsepc_cmd_hdr)
		+ sizeof(struct lsepc_cmd_ftr)
		+ size;

	if (buff_size > PROTO_CMD_SIZE)
		return -EMSGSIZE;

	hdr->enq = PROTO_ENQ;
	hdr->soh = PROTO_SOH;
	hdr->cmd = cmd;
	hdr->stx = PROTO_STX;

	memcpy(buff + sizeof(struct lsepc_cmd_hdr), args, size);

	ftr->eot = PROTO_EOT;


	if (lsepc_getsync(lsepc))
		return -EREMOTEIO;

	if (write(lsepc->dev, buff, buff_size) != buff_size)
		return -ECOMM;

	return 0;
}

static int lsepc_recv_resp(struct lsepc *lsepc, uint8_t *resp, int size)
{
	struct lsepc_resp_hdr hdr;
	struct lsepc_resp_ftr ftr;
	int ret;

	if ((ret = lsepc_read(lsepc->dev, &hdr.ack, 1)) != 0)
		return ret;

	if (hdr.ack == PROTO_NAK)
		return -ECANCELED;
	if (hdr.ack != PROTO_ACK)
		return -EBADMSG;

	if ((ret = lsepc_read(lsepc->dev, &hdr.size, 2)) != 0)
		return ret;

	if (hdr.stx != PROTO_STX)
		return -EBADMSG;

	if (hdr.size > size)
		return -EMSGSIZE;

	if ((ret = lsepc_read(lsepc->dev, resp, hdr.size)) != 0)
		return ret;

	if ((ret = lsepc_read(lsepc->dev, &ftr, sizeof(struct lsepc_resp_ftr))) != 0)
		return ret;

	if (ftr.eot != PROTO_EOT)
		return -EBADMSG;

	return 0;
}

struct lsepc *lsepc_init(const char *dev)
{
	struct lsepc *lsepc = malloc(sizeof(struct lsepc));
	lsepc->dev_name = dev;

	lsepc->dev = open(dev, O_RDWR | O_NOCTTY);
	if (lsepc->dev < 0) {
		warn("lsepc: Cannot open %s", dev);
		free(lsepc);
		return NULL;
	}

	tcgetattr(lsepc->dev, &lsepc->term_org);

	memset(&lsepc->term_cur, 0, sizeof(struct termios));
	lsepc->term_cur.c_cflag = CLOCAL | CS8 | CREAD;
	lsepc->term_cur.c_iflag = 0;
	lsepc->term_cur.c_oflag = 0;
	lsepc->term_cur.c_lflag = 0;
	lsepc->term_cur.c_cc[VTIME] = 0;
	lsepc->term_cur.c_cc[VMIN] = 1;

	cfsetospeed(&lsepc->term_cur, BAUDRATE);
	cfsetispeed(&lsepc->term_cur, BAUDRATE);

	tcflush(lsepc->dev, TCIOFLUSH);

	tcsetattr(lsepc->dev, TCSANOW, &lsepc->term_cur);

	return lsepc;
}



int lsepc_get_version(struct lsepc *lsepc, char *version, int size)
{
	int ret;

	if ((ret = lsepc_send_cmd(lsepc, CMD_GETVERSION, NULL, 0)) < 0)
		return ret;

	return lsepc_recv_resp(lsepc, (uint8_t *) version, size);
}

static int lsepc_single_cmd(struct lsepc *lsepc, uint8_t cmd)
{
	int ret;
	uint8_t cmd_ret;

	if ((ret = lsepc_send_cmd(lsepc, cmd, NULL, 0)) < 0)
		return ret;

	if ((ret = lsepc_recv_resp(lsepc, &cmd_ret, 1)))
		return ret;
	if (cmd_ret)
		return -ENOSYS;
	return 0;
}

int lsepc_stop(struct lsepc *lsepc)
{
	return lsepc_single_cmd(lsepc, CMD_STOP);
}

int lsepc_start(struct lsepc *lsepc)
{
	return lsepc_single_cmd(lsepc, CMD_START);
}

int lsepc_brk(struct lsepc *lsepc)
{
	return lsepc_single_cmd(lsepc, CMD_BRK);
}

int lsepc_cont(struct lsepc *lsepc)
{
	return lsepc_single_cmd(lsepc, CMD_CONT);
}

int lsepc_get_regs(struct lsepc *lsepc, struct lsepc_regs *regs)
{
	int ret;

	if ((ret = lsepc_send_cmd(lsepc, CMD_GETREGS, NULL, 0)) < 0)
		return ret;

	if ((ret = lsepc_recv_resp(lsepc, (uint8_t *) regs,
			sizeof (struct lsepc_regs))))
		return ret;

	return regs->valid;
}

int lsepc_get_status(struct lsepc *lsepc)
{
	int ret;
	uint8_t status;

	if ((ret = lsepc_send_cmd(lsepc, CMD_GETSTATUS, NULL, 0)) < 0)
		return ret;

	if ((ret = lsepc_recv_resp(lsepc, (uint8_t *) &status, 1)))
		return ret;

	return status;
}

int lsepc_set_io_data(struct lsepc *lsepc, uint16_t data)
{
	int ret;
	uint16_t resp;

	if ((ret = lsepc_send_cmd(lsepc, CMD_SETIODATA, (void *) &data, 2)) < 0)
		return ret;

	if ((ret = lsepc_recv_resp(lsepc, (uint8_t *) &resp, 2)) < 0)
		return ret;

	return resp != data ? -EREMOTEIO : 0;
}

int lsepc_get_io_data(struct lsepc *lsepc)
{
	int ret;
	uint16_t resp;

	if ((ret = lsepc_send_cmd(lsepc, CMD_GETIODATA, NULL, 0)) < 0)
		return ret;

	if ((ret = lsepc_recv_resp(lsepc, (uint8_t *) &resp, 2)) < 0)
		return ret;

	return resp;
}

int lsepc_get_io_addr(struct lsepc *lsepc)
{
	int ret;
	uint16_t resp;

	if ((ret = lsepc_send_cmd(lsepc, CMD_GETIOADDR, NULL, 0)) < 0)
		return ret;

	if ((ret = lsepc_recv_resp(lsepc, (uint8_t *) &resp, 2)) < 0)
		return ret;

	return resp;
}

void lsepc_delete(struct lsepc *lsepc)
{
	tcsetattr(lsepc->dev, TCSANOW, &lsepc->term_org);
	close(lsepc->dev);
	free(lsepc);
}
