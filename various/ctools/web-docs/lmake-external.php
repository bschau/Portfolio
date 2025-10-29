<?php
	define("TITLE", "external commands (lmake)");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">LMake - External Commands</H1>
<P>Any token not found in the list of builtin commands, expressions etc. are treated as if they are external commands.
<P>By specifying the <B>@</B> flag the following statement will be forced as an external command.
<P>The rest of the current line is taken as arguments to the external command. Normal substitution will be done on all arguments and the expanded argument list will be supplied to the external command.
<P>Output from the external command will normally be suppressed. Supplying the <B>%</B> flag will override this. The output will also be captured in the <B>$-</B> variable.
<P>The return code of the external command is captured in the <B>$?</B> variable.
<P CLASS="manualnav"><A HREF="lmake-builtins.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lmake-usage.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
