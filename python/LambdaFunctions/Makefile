all:	clean install

install:
	-make -C comics install
	-make -C dkjokes install
	-make -C events install
	-make -C kwed install
	-make -C lichess install
	-make -C newsfeed install

clean:
	-rm -f *~
	-make -C comics distclean
	-make -C dkjokes distclean
	-make -C events distclean
	-make -C kwed distclean
	-make -C lichess distclean
	-make -C newsfeed distclean

distclean: clean
