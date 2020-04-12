<?php
	define("TITLE", "variables (lmake)");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">LMake - Variables</H1>
<P>A variable is a placeholder of some sort. Much like any other variable in any other programming language. A variable in LMake is typeless - it is basically a string and if it is used in a context where a numeric value is required, the string is converted to a numeric value. This is much like your ordinary scripting language.
<P>A variable can come from the local scope, the LMake global scope or even the system scope. The search order is:
<UL>
<LI>Look in the local scope - that is the scope of the current procedure.</LI>
<LI>Look in the global scope - this is the scope of the entire LMakefile and any parent and/or child LMakefiles.</LI>
<LI>Look in the system scope - the scope on the operation system level (process level).</LI>
</UL>
<P>The syntax to define a variable:
<PRE>
    var = val
</PRE>
<P>Valid definitions are:
<PRE>
    myvar = 10
    myvar = "abc"
    myvar = 'abc'
    myvar = $(otherVar)
</PRE>
<P>To use, or reference, a variable, use the syntax as presented in the last line in the examples above (and repeated here):
<PRE>
    myvar = $(otherVar)
</PRE>
<P>This basically says that <B>myvar</B> should have the same content as <B>otherVar</B>
   

<P><BR><P>
<H3 CLASS="topic">Builtin Variables</H3>
<P>The following variables are defined in the global scope for all LMakefiles. Most of the variables can be changed or even undefined if needed.
<DL>
<DT>CC</DT>
<DD>Compiler command. On Unix this is <TT>gcc</TT>, on Windows this is <TT>tcc.exe</TT>.</DD>
<DT>CFLAGS</DT>
<DD>Compiler flags. On Unix this is <TT>-Wall -O2</TT>, on Windows this is <TT>-Wall</TT>.</DD>
<DT>CWD</DT>
<DD>The <I>C</I>urrent <I>W</I>orking <I>D</I>irectory. This is a read only variable.</DD>
<DT>EXE</DT>
<DD>Executable suffix. On Unix this is blank, on Windows this is <TT>.exe</TT>.</DD>
<DT>OS</DT>
<DD>Operating System type. Currently, this returns <TT>Linux</TT> for Linux systems or <TT>WIN32</TT> for Windows systems. <B>Note!</B> This may some day turn into a read only variable.</DD>
<DT>PWD</DT>
<DD>The <I>P</I>arent <I>W</I>orking <I>D</I>irectory. This is a read only variable.</DD>
<DT>RD</DT>
<DD>Remove directory command. On Unix this is <TT>rm -fr</TT>, on Windows this is <TT>rmdir /s /q</TT>.</DD>
<DT>RM</DT>
<DD>Remove file command. On Unix this is <TT>rm -f</TT>, on Windows this is <TT>del /q</TT>.</DD>
<DT>seconds</DT>
<DD>Seconds since epoch (1st of Jan, 1970 on Unix / 1st of Jan, 1980 on Windows). This is a read only variable.</DD>
<DT>SO</DT>
<DD>Shared Objects suffix. On Unix this is <TT>.so</TT>, on Windows this is <TT>.dll</TT>.</DD>
<DT>userid</DT>
<DD>The user id lmake is currently running as. This is a read only variable.</DD>
</DL>

<P CLASS="manualnav"><A HREF="lmake.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lmake-procedures.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
