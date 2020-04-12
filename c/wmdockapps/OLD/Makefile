PREFIX	=	/usr/local

all:	clean install

clean:
	make -C lib clean
	make -C src clean
	make -C man clean
	-rm -f *~

install:	clean
	PREFIX=$(PREFIX) make -C lib install
	PREFIX=$(PREFIX) make -C src install
	PREFIX=$(PREFIX) make -C man install
	sudo /sbin/ldconfig

archive:	clean
	rm -f ../wmdockapps.tar.gz
	(cd ..; tar czvf wmdockapps.tar.gz wmdockapps)
