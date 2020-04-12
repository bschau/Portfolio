<?php
	define("TITLE", "introduction to lme");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">lme - Example</H1>
<H3 CLASS="topic">Properties file</H3>
<PRE>
    VAR1          = "hello, world"
    VAR2          = $(VAR1)
    my_os_string  = $(@os)
</PRE>
<P><BR><P>
<H3 CLASS="topic">Source file</H3>
<PRE>
    This is a small test, created $(@filectime this), modified $(@filemtime this)
    and accessed $(@fileatime this).

    Running on $($HOSTNAME) / $(my_os_string).

    Messages:

            $(VAR1)
            $(VAR2)

    This file is: $(@filesize this) bytes.


    Signed
    $(@userid)
</PRE>
<P><BR><P>
<H3 CLASS="topic">Command</H3>
<PRE>
    lme -p propsFile -o output.txt srcFile
</PRE>
<P><BR><P>
<H3 CLASS="topic">Output</H3>
<PRE>
    This is a small test, created Mon Sep 24 21:14:48 2007, modified Mon Sep 24 21:14:48 2007
    and accessed Mon Sep 24 21:14:56 2007.

    Running on stacy.local / Linux.

    Messages:

            hello, world
            hello, world

    This file is: 245 bytes.


    Signed
    bsc
</PRE>

<P CLASS="manualnav"><A HREF="lme-substitutions.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. &gt;&gt;
</DIV>
</BODY>
</HTML>
