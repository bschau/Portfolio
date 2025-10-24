CC	=	gcc
INCS	=	-I . -I /usr/local/include/snacc/c/
LIBS	=	/usr/local/lib/libasn1csbuf.a
BER	=	c:/temp/family.ber
CFLAGS	=	-O2 -Wall $(INCS) -DUSE_SBUF -DFAMILYBER="\"$(BER)\""
ASN1	=	familyrecord

all:	$(ASN1).c fq tober

$(ASN1).c:	$(ASN1).asn1
	snacc -u /usr/local/include/snacc/asn1/asn-useful.asn1 $(ASN1).asn1

fq:	fq.o $(ASN1).o
	$(CC) $(CFLAGS) $(LIBPATH) -o fq fq.o $(ASN1).o $(LIBS)

tober:	tober.o $(ASN1).o
	$(CC) $(CFLAGS) $(LIBPATH) -o tober tober.o $(ASN1).o $(LIBS)

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f fq.exe fq tober.exe tober *.o *~ *.stackdump $(BER)

distclean:	clean
	rm -f $(ASN1).c $(ASN1).h

ex:	all
	./tober
	xxd $(BER)
