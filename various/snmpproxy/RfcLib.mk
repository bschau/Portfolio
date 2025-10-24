include ../TopMake.mk
ASN1SRC		=	../asn1/rfc1155.asn1 ../asn1/rfc1157.asn1
AUTOGENS	=	rfc1155.c rfc1155.h rfc1157.c rfc1157.h
OBJS		=	rfc1155.o rfc1157.o
DST		=	rfclib.a

all:	$(DST)

clean:
	rm -f *~ $(DST) *.o

distclean: clean
	rm -f $(AUTOGENS)

$(AUTOGENS):
	$(ASN1CC) $(ASN1ARGS) $(ASN1SRC)
	(cp rfc1155.h rfc1155.t; sed -f ../scripts/asn1uint.sed rfc1155.t > rfc1155.h; rm -f rfc1155.t)

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

$(DST):	$(AUTOGENS) $(OBJS)
	$(AR) -cr $(DST) $(OBJS)
	$(RANLIB) $(DST)
