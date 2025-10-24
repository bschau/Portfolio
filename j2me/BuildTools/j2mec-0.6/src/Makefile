CC	=	javac
CFLAGS	=	-g:none
JAR	=	jar
JARARGS	=	cvfm
OBJ	=	j2mec.class
SRCS	=	j2mec.java Compiler.java Expander.java JarManager.java PathManager.java PreProcessor.java ProcessManager.java Variables.java
DST	=	j2mec

all:	$(DST)

initall:
	mkdir -p $(DST)
	ln -sf scripts/test.sh x
	make -C ../examples/HelloWorld initall
	make -C ../examples/Test initall
	make -f Makefile.JavaDoc initall

$(DST):	$(OBJ)

$(OBJ):	$(SRCS)
	$(CC) $(CFLAGS) -d $(DST) $(SRCS)

clean:
	rm -f $(DST)/* $(DST).jar
	rm -f scripts/*~ *~

distclean:	clean
	rm -fr $(DST) x
	make -f Makefile.JavaDoc distclean

jar:	$(DST)
	(cd $(DST); $(JAR) $(JARARGS) ../$(DST).jar ../MANIFEST *.class)

javadoc:
	make -f Makefile.JavaDoc
