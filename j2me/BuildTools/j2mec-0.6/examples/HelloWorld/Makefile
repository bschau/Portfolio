DST	=	HelloWorld

$(DST):
	java -cp ../../src/j2mec j2mec

j2mec:
	java -jar /usr/local/j2mec/current/j2mec.jar

initall:
	mkdir -p tmp

clean:
	rm -f $(DST).jar $(DST).jad
	find . -type f -name "*~" -exec rm -f {} \;

distclean:	clean
	rm -fr tmp
