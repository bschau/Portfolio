<?php
	define("TITLE", "example (lmake)");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">LMake - Example</H1>
<P>Remember the example from the introduction? It is repeated here, in a commented form:
<P>
<PRE>
    # Setup the DST variable. Due to the $(EXE) suffix DST will become 'ell' on
    # Linux and 'ell.exe' on Windows.
    DST         =       ell$(EXE)
    SOURCES     =       Main Ell Stack
    DEPS        =       ..$/Include.h
    # CFLAGS - On Linux = "-DLinux -I .."   On Windows = "-DWIN32 -I .."
    CFLAGS      =       -D$(OS) -I ..

    # The all procedure. If I run lmake with no arguments, lmake will run the
    # 'all' procedure which invokes lmake as:
    all:
            lmake -C build -f ../LMakefile -DBUILDING build 

    initall:
            test -d build || mkdir build

    # A custom procedure.  Setup a list $(SOURCES) and run compiles. In this
    # case the list is expanded to:
    #
    # compile Main
    # compile Ell
    # compile Stack
    #
    # Then 'ell'/'ell.exe' (if found) is validated agains Main.o Ell.o Stack.o
    # to see if it is newer. If not newer or not found $(CC) is called.
    build:
            &lt; $(SOURCES) &gt; compile
            depend $(DST) $(SOURCES).o || $(CC) $(CFLAGS) -o ..$/$(DST) $(SOURCES).o

    # $&lt;.o (Main.o, Ell.o, Stack.o) is generated if they are not found or if
    # they're older than their .c counterparts.
    compile:
            depend $&lt;.o ..$/$&lt;.c $(DEPS) || $(CC) $(CFLAGS) -c ..$/$&lt;.c

    clean:
            $(RM) build$/*.o
            $(RM) $(DST)
            $(RM) *~

    distclean:	clean
            $(RD) build
</PRE>
<P>Now, that wasn't so hard, eh?

<P CLASS="manualnav"><A HREF="lmake-usage.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lcdoc.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
