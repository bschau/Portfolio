CC		=	gcc
INCS		=	-I . -I /usr/local/include/snacc/c -DUSE_SBUF
LIBS		=	/usr/local/lib/libasn1csbuf.a
CFLAGS		=	-O2 -Wall $(INCS) -g
ASN1CC		=	snacc
ASN1ARGS	=	-u /usr/local/include/snacc/asn1/asn-useful.asn1
RANLIB		=	ranlib
