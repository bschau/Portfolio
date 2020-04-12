PREFIX	?=	/usr/local
DST	=	libwmdockapps.so
CC	=	gcc
CFLAGS	=	-Wall -I. -O2 -fPIC
LDFLAGS	=	-lX11 -lXpm -shared -fPIC
SRCS	=	wmdockapps.c
OBJS	=	$(SRCS:.c=.o)

all:	$(DST)

$(DST):	$(OBJS)
	$(CC) $(LDFLAGS) -o $(DST) $(OBJS)

.c.o:	$(DEPS)
	$(CC) $(CFLAGS) -c $<

install:	$(DST)
	sudo cp libwmdockapps.so $(PREFIX)/lib
	sudo cp wmdockapps.h $(PREFIX)/include
	sudo /sbin/ldconfig

clean:
	rm -f $(OBJS) $(DST) *~
