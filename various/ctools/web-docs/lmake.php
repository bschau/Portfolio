<?php
	define("TITLE", "introduction to lmake");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">LMake - Introduction</H1>
<P><I>lmake</I> is a <B>make</B> replacement. lmake operates on one or more LMakefiles. A LMakefile contains one or more <I>procedures</I> (sometimes called "rules" in make terminology).
<P>A procedure is a set of instructions, which, when called performs one or more subtasks or steps.
<P>A step can be a variable manipulation, a call to a builtin command, a call to another procedure or a call to a system command.
<P>To wet your teeth, a sample <TT>LMakefile</TT> looks like the following:
<PRE>
    DST         =       ell$(EXE)
    SOURCES     =       Main Ell Stack
    DEPS        =       ..$/Include.h
    CFLAGS      =       -D$(OS) -I ..

    all:
            lmake -C build -f ../LMakefile -DBUILDING build 

    initall:
            test -d build || mkdir build

    build:
            &lt; $(SOURCES) &gt; compile
            depend $(DST) $(SOURCES).o || $(CC) $(CFLAGS) -o ..$/$(DST) $(SOURCES).o

    compile:
            depend $&lt;.o ..$/$&lt;.c $(DEPS) || $(CC) $(CFLAGS) -c ..$/$&lt;.c

    clean:
            $(RM) build$/*.o
            $(RM) $(DST)
            $(RM) *~

    distclean:	clean
            $(RD) build
</PRE>
<P>The above <TT>LMakefile</TT> defines 6 procedures:
<UL>
<LI><B>all</B> - the default procedure.</LI>
<LI><B>initall</B> - create a build directory.</LI>
<LI><B>build</B> - compiles the source code files defined in $(SOURCES) and link the compiled files into one binary file, $(DST).</LI>
<LI><B>compile</B> - a sub-procedure to <B>build</B> which compiles a source code file if the source code has changed.</LI>
<LI><B>clean</B> - remove build files and temporary files.</LI>
<LI><B>distclean</B> - really clean up everything.</LI>
</UL>
<P>The various options and how to define procedures and so on will be described in the next chapters.
<P><BR><P>
<H3 CLASS="topic">Syntax</H3>
<P>LMakefiles are based on <I>words</I> or <I>tokens</I>. A token is an identifier (<B>ABC</B>), a string (<B>"hello"</B>, <B>'abc'</B> or just <B>abc</B>), a command or just about anything.
<P>Tokens <B>MUST</B> be separated by one or more whitespaces - plain blanks or tabs.
<P>Tokens and identifiers are case-sensitive. <B>abc</B> is not the same as <B>ABC</B> which is not the same as <B>AbC</B>.
<P>Comments are started with a <B>#</B> (hash sign). Comments can start anywhere on the line and runs to the end of the line. A # in a string is not treated as a comment.

<P CLASS="manualnav"><A HREF="instwin.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lmake-variables.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
