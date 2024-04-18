A	=	$(shell basename `pwd`)

clean:
	find . -type f -name "*~" -exec rm -f {} \;
	rm -fr tmp
	rm -f files.lst

release:	clean
	@./CreateFileList.sh
	(cd ..; tar czvf FootMail.tar.gz -T $(A)/files.lst)

archive:	clean
	rm -f config.inc.php FootMail-$(A).tar.gz files.lst
	(cd ..; tar cjf $(A).tar.bz2 $(A))
