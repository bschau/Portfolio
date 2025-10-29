<?php
	define("TITLE", "introduction to lme");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">lme - Substitutions</H1>
<P>lme can substitute more than just variables - the following substitutions and commands exists:
<DL>
<DT>var</DT>
<DD>Substitute with the value of var.</DD>
<DT>$var</DT>
<DD>Get value of system environment variable var.</DD>
<DT>=file</DT>
<DD>Get content of file.</DD>
<DT>!cmd [args]</DT>
<DD>Get the output from the command "cmd args".</DD>
<DT>@cmd</DT>
<DD>Get the output of the builtin command (see below).</DD>
</DL>
<P>If a invalid substitution is given an error is issued and no further processing is done.

<P><BR><P>
<H3 CLASS="topic">Builtin Commands</H3>
<P>The following builtin commands are defined:
<DL>
<DT>os</DT>
<DD>Return the operating system token - currently <B>Linux</B> or <B>WIN32</B>.</DD>
<DT>userid</DT>
<DD>The user id of the user.</DD>
<DT>seconds</DT>
<DD>Number of seconds since the epoch (1970-01-01 on Linux and 1980-01-01 on Windows).</DD>
<DT>ctime</DT>
<DD>Returns the current time/date in the form such as "Wed Jun 30 21:49:09 1993"</DD>
<DT>filesize file</DT>
<DD>Returns the size of <TT>file</TT>. <TT>file</TT> can be an absolute path, a partial path or the special keyword <B>this</B> denoting the source file.</DD>
<DT>fileatime[g] file [fmt]</DT>
<DD>Return the last accessed time of <TT>file</TT>. <TT>file</TT> can be an absolute path, a partial path or the special keyword <B>this</B> denoting the source file. The <TT>fmt</TT> is optional and specifies a formatting string - use <TT>man strftime</TT> on linux to see the the format specifiers or use <A HREF="http://www.google.com/search?q=strftime">Google</A>. If <TT>fmt</TT> is omitted <TT>fileatime</TT> behaves as <TT>ctime</TT>. If <TT>fileatimeg</TT> is used the time returned is GM Time.</DD>
<DT>filectime[g] file [fmt]</DT>
<DD>Return the created time of <TT>file</TT>. See <TT>fileatime</TT> for more information.</DD>
<DT>filemtime[g] file [fmt]</DT>
<DD>Return the last modified time of <TT>file</TT>. See <TT>fileatime</TT> for more information.</DD>
</DL>

<P CLASS="manualnav"><A HREF="lme-properties.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lme-example.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
