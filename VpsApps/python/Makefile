all:	clean lint

init:
	pip3 install -r requirements.txt

lint:
	-pylint3 --rcfile=pylintrc src/*.py

clean:
	-find . -type f -name "*~" -exec rm -f {} \;
	-rm -fr src/__pycache__
	-rm -f src/*.pyc

distclean:	clean

deploy:	clean lint
	scp events.json newsfeeds.json requirements.txt src/*.py $(VPSAPPSACCOUNT):/home/bs/Apps
