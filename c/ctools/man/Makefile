MANS	=	$(shell ls *.1)
P	=	$(PREFIX)/../man/man1

all:	clean install

clean:
	-$(RM) *~

install:
	-mkdir -p $(P)
	$(foreach var,$(MANS), cp $(var) $(P);)
