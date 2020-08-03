FUNCTION	=	qotd

all:	clean lint

init:
	pip3 install feedparser
	pip3 install requests

lint:
	-pylint --rcfile=pylintrc src/*.py

clean:
	-find . -type f -name "*~" -exec rm -f {} \;
	-rm -fr src/__pycache__
	-rm -f src/*.pyc

distclean:	clean

deploy:	clean lint
	scp newsfeedrc src/*.py $(VPSAPPSACCOUNT):/home/bs/Apps
