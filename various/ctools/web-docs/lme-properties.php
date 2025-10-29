<?php
	define("TITLE", "introduction to lme");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">lme - Properties Files</H1>
<P>A properties files is much like the properties files found in the Java environment. They consists of several <TT>variable = value</TT> pairs which for each variable define exactly one value.
<P>The value itself can point to another variable, so some sort of recursion can take place. Variables are not read and defined until they are used.
<P>Multiple properties files can be given on the command line. The last define variable are used if more than one instance of the variable are defined.
<P>Variables are define as:
<PRE>
    variable = value
</PRE>
<P>There must be at least one whitespace on each side of the '='. <TT>value</TT> is one word and one word only. If you need to have whitespaces (tabs, spaces etc.) you need to quote the value in either single or double quotes.
<P>A quoted word can span multiple lines.
<P>Comments are started with <TT>#</TT> and the comment must start on the first character of a line. Comments spans through the entire line.
<P>Example of a properties file:
<PRE>
    #
    # MainClass and StartClass points to the same value.
    #
    MainClass = Main.java
    StartClass = $(MainClass)

    # This key has a value which spans more lines
    MORELINES = "

          spanning more lines 

    "
    # EOF
</PRE>

<P CLASS="manualnav"><A HREF="lme.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lme-substitutions.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
