TGTS	=	aceyducey adventure catch craps galaxypatrol gravedigger juggle nicomacus survive tictactoe wargame
DST	=	/usr/local/bin

all:	c-install

init:
	pip3 install pgzrun
	sudo apt install openjdk-11-jdk

c-install:
	@for f in ${TGTS}; do \
		make -C "$${f}/c" install DDST=$(DST); \
	done

go-install:
	@for f in ${TGTS}; do \
		(echo "Installing $${f}"; cd $${f}/go; go mod tidy; go install -ldflags '-s') \
	done


distclean:
	@for f in ${TGTS}; do \
		make -C $${f}/c distclean ; \
		(cd $${f}/go; go clean) \
	done
	make -C meteor distclean
	(cd gameoflife; ant clean)
	(cd jstar; ant clean)
	find . -iname "*~" -type f -exec rm -f {} \;

clean:	distclean
