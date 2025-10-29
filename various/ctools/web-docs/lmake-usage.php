<?php
	define("TITLE", "usage (lmake)");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">LMake - Usage</H1>
<H3 CLASS="topic">Synopsis</H3>
<PRE>
    Usage: lmake [OPTIONS] [procedure1 ... procedureX]

    [OPTIONS]
      -C dir        Change to directory before processing files.
      -D var[=val]  Define variable. If val is omitted, val is set to 1.
      -U var        Undefine variable.
      -V            Print version string and exit.
      -d            Dry run, don't do anything.
      -f file       Use file as LMakefile.
      -h            This page.
      -v            Raise verbosity.

    If [procedure] is omitted the 'all' procedure is called.
</PRE>
<P><BR><P>
<H3 CLASS="topic">Options Explained</H3>
<DL>
<DT>-C dir</DT>
<DD>Change to directory before processing files. Normally the current directory is searched for a LMakefile to process. By using the <TT>-C</TT> switch the current directory is changed to <B>dir</B> before searching for a LMakefile to process.</DD>
<DT>-D var[=val]</DT>
<DD>Define a new variable and optionally set it to <TT>val</TT>. If <TT>val</TT> is not specified the new variable will be set to <B>1</B>.</DD>
<DT>-U var</DT>
<DD>Undefine (delete) a variable.</DD>
<DT>-V</DT>
<DD>Print version string and exit.</DD>
<DT>-d</DT>
<DD>Dry run, don't do anything. All procedures will succeed, external programs will not be run (but treated as if they ran successfully). This will give a list of what will happen without doing it.</DD>
<DT>-f file</DT>
<DD>Use <B>file</B> as LMakefile. <B>file</B> can be a complete path but the current directory will not be changed.</DD>
<DT>-h</DT>
<DD>Show the help page.</DD>
<DT>-v</DT>
<DD>Raise verbosity. Mostly for debugging purposes.</DD>
</DL>
<P><BR><P>
<H3 CLASS="topic">Procedures</H3>
<P>You can specify one or more procedures to call on the command line. You can also specify one or more arguments to the procedures. If lmake is invoked without specifying a procedure to call, the <B>all</B> procedure is called. If the <B>all</B> procedure does not exist, lmake will fail.
<P>A procedure name (on the command lone) has the following syntax:
<PRE>
    procedurename[:["]arguments["]]
</PRE>
<P>Examples on invocation:
<DL>
<DT>lmake</DT>
<DD>No procedures given, call the <B>all</B> procedure.</DD>
<DT>lmake MyProc</DT>
<DD>Call the <B>MyProc</B> procedure.</DD>
<DT>lmake MyProc AnotherProc LastProc</DT>
<DD>First call the <B>MyProc</B> procedure, then the <B>AnotherProc</B> procedure and lastly the <B>LastProc</B> procedure.</DD>
<DT>lmake MyProc:myarg</DT>
<DD>Call the <B>MyProc</B> procedure with one argument, <TT>myarg</TT>. The procedure will see <TT>myarg</TT> as <TT>$1</TT>.</DD>
<DT>lmake MyProc:"myarg1 myarg2 myarg3"</DT>
<DD>Call the <B>MyProc</B> procedure with three arguments, <TT>myarg1</TT>, <TT>myarg2</TT> and <TT>myarg3</TT>. The procedure will see the arguments in <TT>$1</TT>, <TT>$2</TT> and <TT>$3</TT>.</DD>
</DL>

<P CLASS="manualnav"><A HREF="lmake-external.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lmake-example.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
