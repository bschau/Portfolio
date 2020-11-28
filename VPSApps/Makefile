all:	clean lint

init:
	-rm -fr package
	pip3 install -r requirements.txt
	sudo apt install python3-dateutil

lint:
	-pylint3 --rcfile=pylintrc src/*.py

clean:
	-find . -type f -name "*~" -exec rm -f {} \;
	-rm -fr src/__pycache__
	-rm -f src/*.pyc

distclean:	clean

deploy:	clean lint
	scp src/*.py events.json newsfeedrc $(VPSAPPSACCOUNT):/home/bs/Apps
