THISDIR = $(shell pwd)

CFLAGS += -Os -Wall -I. -std=gnu99 -D_GNU_SOURCE $(EXTRACFLAGS)
LDFLAGS += -s
LIBS = -lresolv

OBJS = 6relayd.o router.o dhcpv6.o ndp.o md5.o dhcpv6-ia.o

all: 6relayd

6relayd: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o 6relayd

romfs:
	$(STRIP) 6relayd
	$(ROMFSINST) /usr/sbin/6relayd