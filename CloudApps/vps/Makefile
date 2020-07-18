FUNCTION	=	qotd

all:	clean lint

init:
	pip3 install feedparser
	pip3 install request

lint:
	-pylint --rcfile=pylintrc email/*.py

clean:
	-find . -type f -name "*~" -exec rm -f {} \;
	-rm -fr email/__pycache__
	-rm -f email/*.pyc

distclean:	clean

deploy:	clean lint
	scp email/newsfeedrc email/sayings.txt email/*.py $(VPSAPPSACCOUNT):/home/bs/Apps
