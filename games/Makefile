TGTS	=	aceyducey adventure catch craps galaxypatrol gravedigger juggle nicomacus survive tictactoe wargame
CDST	=	/usr/local/bin

all:
	echo "Use 'make call' for c-based games or 'make goall' for go-based games"

call:	cinstall

goall:	goinstall

cinstall:
	@for f in ${TGTS}; do \
		make -C "$${f}/c" install DDST=$(DST); \
	done

goinstall:
	@for f in ${TGTS}; do \
		(cd "$${f}/go"; go install -ldflags '-s' ./...) \
	done

distclean:
	@for f in ${TGTS}; do \
		make -C "$${f}/c" distclean ; \
		(cd "$${f}/go"; go clean) \
	done
	find . -iname "*~" -type f -exec rm -f {} \;

clean:	cdistclean
