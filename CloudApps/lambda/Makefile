all:	clean

clean:
	-find . -type f -name "*~" -exec rm -f {} \;
	-make -C comics distclean
	-make -C eventor distclean
	-make -C lichess distclean
	-make -C qotd distclean
	-make -C test distclean
