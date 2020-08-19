FUNCTION	=	comics

all:	clean lint

init:
	-rm -fr package
	pip3 install --system --target=./package -r requirements.txt

lint:
	-pylint3 --rcfile=../pylintrc *.py

clean:
	-find . -type f -name "*~" -exec rm -f {} \;
	-rm -fr __pycache__
	-rm -f function.zip
	-rm -f *.pyc

distclean:	clean
	-rm -fr package

deploy:	clean lint
	(cd package; zip -r9 ../function.zip .)
	zip -g function.zip $(FUNCTION).py mailgun.py
	aws lambda update-function-code --function-name=$(FUNCTION) --zip-file=fileb://function.zip
