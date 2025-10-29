PLUGIN	=	archiver
VERS	=	1.0

release:	clean
	-make -C lang clean
	-make -C lang all
	mkdir $(PLUGIN)
	cp archiver.php $(PLUGIN)
	cp archiverrunner.php $(PLUGIN)
	cp lang/*.mo $(PLUGIN)
	cp LICENSE $(PLUGIN)
	cp README $(PLUGIN)
	zip -r $(PLUGIN).zip $(PLUGIN)/*

pot:
	xgettext --language=PHP --indent --keyword=__ --keyword=_e --keyword=__ngettext:1,2 -s -n --from-code=UTF8 -o messages.pot archiver.php

clean:
	-rm -f *~
	-rm $(PLUGIN).zip
	-rm -fr $(PLUGIN)
	-make -C lang clean

backup:	clean
	(cd ..; zip -r $(PLUGIN)-$(VERS).zip $(PLUGIN)*)
	
