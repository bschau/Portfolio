CC	=	gcc
CFLAGS	=	-O2 -Wall -I.
DST	=	public
OBJS	=	blowfish.o main.o md5.o
DEPS	=	Makefile public.h

all:	$(DST)

clean:
	rm -f $(OBJS) $(DST)

$(DST):	$(DEPS) $(OBJS)
	$(CC) $(CFLAGS) -o $(DST) $(OBJS)

$(OBJS):	%.o: %.c $(DEPS)
