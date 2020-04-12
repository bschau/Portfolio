all:	clean lint

init:
	pip3 install -r requirements.txt

lint:
	-pylint --rcfile=pylintrc python/*.py

clean:
	-find . -type f -name "*~" -exec rm -f {} \;
	-rm -fr python/__pycache__

install:	clean lint
	scp $(VPSAPPSPORT) requirements.txt python/*.py $(VPSAPPSACCOUNT):/home/bs/Apps
