FUNCTION	=	qotd

all:	clean lint

init:
	pip3 install feedparser
	pip3 install request

lint:
	-pylint3 --rcfile=pylintrc python/*.py

clean:
	-find . -type f -name "*~" -exec rm -f {} \;
	-rm -fr python/__pycache__
	-rm -f python/*.pyc

distclean:	clean

deploy:	clean lint
	scp python/newsfeedrc python/sayings.txt python/*.py $(VPSACCOUNT):/home/bs/Apps
