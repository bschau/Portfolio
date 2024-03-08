<?php
	define("TITLE", "procedures (lmake)");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">LMake - Procedures</H1>
<P>Procedures are a collection of steps to take. Or, more formally, a procedure specifies a worklist, a combination of internal or external commands to run. Also, other procedures can be called.
<P>A procedure can receive arguments and treat these as if they were defined within the procedure.
<P>A procedure is defined as:
<PRE>
    procedurename:
    stuff_to_do_1
    stuff_to_do_2
    .
    .
</PRE>
<P>A procedure is terminated by one or more blank lines. Examples of procedure definitions:
<PRE>
    proc1:
    do_stuff_1
    do_stuff_2

    proc2:
        do_stuff_1
        do_stuff_2

   proc3: do_stuff_1
        do_stuff_2
</PRE>
<P>As the above procedures shows, the steps may or may not be indented. Also, one or more steps can be specified right after the procedure definition - more on this later.
<P>Procedures can receive arguments. These are addressed as <TT>$0</TT>, <TT>$1</TT>, ...
<P><TT>$0</TT> is a special argument - it denotes all the arguments. Example:
<PRE>
    myproc a b c
</PRE>
<P><B>myproc</B> will be called, the arguments are:
<PRE>
    $0   a b c
    $1   a
    $2   b
    $3   c
</PRE>

<P><BR><P>
<H3 CLASS="topic">The all procedure</H3>
<P>The <B>all</B> procedure is a special system define procedure. It may or may not be defined. If defined, it will be called if lmake is invoked without arguments.

<P CLASS="manualnav"><A HREF="lmake-variables.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lmake-substitutions.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
