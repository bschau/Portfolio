FUNCTION	=	newsfeed

all:	clean lint

init:
	-rm -fr package
	pip3 install --target=./package -r requirements.txt
	cp ~/.lambda.json settings.json

lint:
	-pylint --rcfile=../pylintrc *.py

clean:
	-find . -type f -name "*~" -exec rm -f {} \;
	-rm -fr __pycache__
	-rm -f function.zip
	-rm -f *.pyc

distclean:	clean
	-rm -fr package settings.json

deploy:	clean lint
	(cd package; zip -r9 ../function.zip .)
	zip -g function.zip $(FUNCTION).py sendmail.py settings.json feeds.json
	aws lambda update-function-code --function-name=$(FUNCTION) --zip-file=fileb://function.zip

install: distclean init deploy

