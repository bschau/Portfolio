<?php
	define("TITLE", "builtin commands (lmake)");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">LMake - Builtin Commands</H1>
<P>A builtin command is a special command defined by lmake. These are:
<DL>
<DT>&lt; ... &gt; procedure</DT>
<DD>Set up a list. The list will be iterated and the specified procedure will be called one time per item in the list. The current item can be addressed in the procedure with <TT>$&lt;</TT>. Example: <TT>&lt; a b c &gt; myproc</TT> - will call <B>myproc</B> three times and setting <TT>$&lt;</TT> to <B>a</B> on the first call, <B>b</B> on the second call and <B>c</B> on the last call.</DD>
<DT>if expression</DT>
<DD>Do something if <B>expression</B> is true.</DD>
<DT>ifdef var</DT>
<DD>Do something if <B>var</B> is defined.</DD>
<DT>ifndef var</DT>
<DD>Do something if <B>var</B> is not defined.</DD>
<DT>else</DT>
<DD>If one of the above <B>if</B>'s evaluated to false, then do this instead.</DD>
<DT>endif</DT>
<DD>Close if/ifdef/ifndef block.</DD>
<DT>include file</DT>
<DD>Include (read and parse) the content of <B>file</B>.</DD>
<DT>&amp;&amp; statements</DT>
<DD>Do statements if the previous statement returned true.</DD>
<DT>|| statements</DT>
<DD>Do statements if the previous statement returned false.</DD>
<DT>in directory [ procedure | command ]</DT>
<DD>Run the given <B>procedure</B> or <B>command</B> in the specified <B>directory</B>.</DD>
<DT>msg message</DT>
<DD>Write <B>message</B> to standard output.</DD>
<DT>test expression</DT>
<DD>Evaluate <B>expression</B> - the result can be reacted upon by &amp;&amp; and ||.</DD>
<DT>vardump</DT>
<DD>Dump all contents of variables and procedures to standard output.</DD>
<DT>depend target source1 ... sourceN</DT>
<DD>Evaluate to true if <B>target</B> file is newer than any of the <B>source1 ... sourceN</B> files.</DD>
<DT>lmake</DT>
<DD>Run lmake as if it was invoked on the command line.</DD>
<DT>writefile file i1 ... iX</DT>
<DD>Write the content of <B>i1 ... iX</B> to <B>file</B>.</DD>
<DT>readfile var file1 ... fileX</DT>
<DD>Read the content of files <B>file1 ... fileX</B> into <B>var</B>.</DD>
<DT>exit [code]</DT>
<DD>Exit now, optionally with <B>code</B>. If <B>code</B> is not set, exit with 0.</DD>
<DT>basename var path1 ... pathX</DT>
<DD>Return the base name component of <B>path1 ... pathX</B> into <B>var</B>.</DD>
</DL>

<P><BR><P>
<H3 CLASS="topic">Expressions</H3>
<P>The following expressions to the <B>if</B> and <B>test</B> commands are:
<DL>
<DT>-d zzz</DT>
<DD>Return true if <B>zzz</B> is a directory.</DD>
<DT>-e zzz</DT>
<DD>Return true if <B>zzz</B> exists.</DD>
<DT>-f zzz</DT>
<DD>Return true if <B>zzz</B> is a file.</DD>
<DT>-n zzz</DT>
<DD>Return true if <B>zzz</B> is a string with content (eg. a non-empty string).</DD>
<DT>-s zzz</DT>
<DD>Return true if <B>zzz</B> is a file with content (eg. a non-zero length file).</DD>
<DT>-z zzz</DT>
<DD>Return true if <B>zzz</B> is an empty string.</DD>
<DT>aaa -et bbb</DT>
<DD>Return true if file <B>aaa</B> is, timewise, equal to file <B>bbb</B>.</DD>
<DT>aaa -nt bbb</DT>
<DD>Return true if file <B>aaa</B> is newer than file <B>bbb</B>.</DD>
<DT>aaa -ot bbb</DT>
<DD>Return true if file <B>aaa</B> is older than file <B>bbb</B>.</DD>
<DT>aaa == bbb</DT>
<DD>Return true if string <B>aaa</B> is equal to string <B>bbb</B>.</DD>
<DT>aaa != bbb</DT>
<DD>Return true if string <B>aaa</B> is not equal to string <B>bbb</B>.</DD>
</DL>

<P><BR><P>
<H3 CLASS="topic">Flags</H3>
<P>Before each statement various flags are set up. The flags tells lmake how to treat the following statement:
<DL>
<DT>-</DT>
<DD>Always treat the statement as if it succeeded.</DD>
<DT>%</DT>
<DD>Force output. Normally lmake "swallows" the output of the individual commands (they're stored in <TT>$-</TT>). By setting the <TT>%</TT> flag output will also be written to standard output.</DD>
<DT>!</DT>
<DD>Suppress output of command. Normally the command run is written to standard output but by setting the <TT>!</TT> flag, the command to run will not be written to standard output.</DD>
<DT>*</DT>
<DD>Treat the following statement as a procedure name to call. Normally, if you define a procedure as:<BR>
<PRE>
    proc1: proc2 proc3
</PRE>
<BR>lmake will, when invoked as <TT>lmake proc1</TT>, call <B>proc2</B> with <B>proc3</B> as an argument. However, if you define proc1 as:<BR>
<PRE>
    proc1: proc2 *proc3
</PRE>
<BR>and invoke lmake as <TT>lmake proc1</TT>, lmake will first call <B>proc2</B> and then call <B>proc3</B>.</DD>
<DT>@</DT>
<DD>Force lmake to treat the following statement as an external command to run.</DD>
</DL>
<P>

<P CLASS="manualnav"><A HREF="lmake-substitutions.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lmake-external.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
