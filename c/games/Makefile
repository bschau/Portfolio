TGTS	=	aceyducey adventure catch craps galaxypatrol gravedigger juggle nicomacus survive tictactoe wargame
DST	=	/usr/local/bin

all:	install

install:
	@for f in ${TGTS}; do \
		make -C "$${f}" install DDST=$(DST); \
	done

distclean:
	@for f in ${TGTS}; do \
		make -C $${f} distclean ; \
	done
	find . -iname "*~" -type f -exec rm -f {} \;

clean:	distclean
