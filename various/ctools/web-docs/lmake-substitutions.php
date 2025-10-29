<?php
	define("TITLE", "substitutions (lmake)");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">LMake - Substitutions</H1>
<P>So, what is a substitution. You've seen substitutions in variables <TT>$(..)</TT> and arguments to procedures <TT>$x</TT>. Substitutions are started with the <TT>$</TT> character.
<P>Other substitutions exist:
<DL>
<DT>\x</DT>
<DD>Pass on <B>x</B> without doing any substitutions. F.ex. if you want to pass a $ you would use <TT>\$</TT>.</DD>
<DT>$(var)</DT>
<DD>Expands to the content of the variable, <TT>var</TT>.</DD>
<DT>$-</DT>
<DD>Expands to the output of the last command.</DD>
<DT>$&lt;</DT>
<DD>Expands to the current file, if a file has been defined (typically by the list builtin).</DD>
<DT>$?</DT>
<DD>Expands to the return code of the last command.</DD>
<DT>$0</DT>
<DD>Expands to the full argument list.</DD>
<DT>$1 ..</DT>
<DD>Expands to the individual arguments in a argument list.</DD>
<DT>$/ or $\</DT>
<DD>Expands to the path separator for the current operating system. If you use either form in your paths they will always be "correct" no matter the platform. This is mostly useful for relative paths. Example: <TT>..$/dir1$/dir2/$dir3</TT> - no matter the platform, <TT>dir3</TT> will always be found (if it exists).</DD>
<DT>$[prefix val1 val2 ... valN]</DT>
<DD>Expands to val1 - valN prefixed with prefix (see below).</DD>
</DL>

<P><BR><P>
<H3 CLASS="topic">Concatenation</H3>
<P>The <B>.</B> (dot) operator at most time serves a special purpose. On token level it is used to concatenate two other tokens without removing the dot:
<PRE>
    a.b
</PRE>
<P>... will return <B>a.b</B>
<PRE>
    SRC = a b c d
    e = $(SRC).o
</PRE>
<P>... will set <TT>e</TT> to <B>a.o b.o c.o d.o</B>

<P><BR><P>
<H3 CLASS="topic">Square bracket list</H3>
<P>The square bracket list is a list where item 2 to item X are prefixed with item 1:
<PRE>
    $[/tmp/ a b c d]
</PRE>
<P>... will convert the list into <TT>/tmp/a /tmp/b /tmp/c /tmp/d</TT>.

<P CLASS="manualnav"><A HREF="lmake-procedures.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lmake-builtins.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
