all:	clean lint

init:
	pip3 install -r requirements.txt

lint:
	-pylint3 --rcfile=pylintrc src/*.py

clean:
	-find . -type f -name "*~" -exec rm -f {} \;
	-rm -fr src/__pycache__

deploy:	clean lint
	scp $(VPSAPPSPORT) requirements.txt src/*.py $(VPSAPPSACCOUNT):/home/bs/Apps
	scp $(VPSAPPSPORT) sayings.txt $(VPSAPPSACCOUNT):/home/bs/.sayings
	scp $(VPSAPPSPORT) events.txt $(VPSAPPSACCOUNT):/home/bs/.events

install: clean lint
	cp requirements.txt src/*.py /home/bs/Apps
	cp sayings.txt /home/bs/.sayings
	cp events.txt /home/bs/.events
