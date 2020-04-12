<?php
	define("TITLE", "installation on Windows");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">Installation on Windows</H1>
<P>To install <I>ltools</I> on Windows follow these steps:
<OL>
<LI>Run the <TT>ltools-x.y.exe</TT> binary.</LI>
<LI>Select <B>Next</B> a couple of times.</LI>
<LI>When installation has ended please add the <I>ltools</I> target directory to your system path.</LI>
</OL>
<P>And that is it!

<P><BR><P>
<A NAME="build"></A><H3 CLASS="topic">Building from source</H3>
<P>To build from source, follow these steps:
<OL>
<LI>You need to have Fabrice Bellards excellent <A HREF="http://fabrice.bellard.free.fr/tcc/">Tiny C Compiler (TCC)</A> installed.
<LI>Unpack the <TT>ltools-x.y.tar.gz</TT> archive using a tool such as Winzip, or, if you have Cygwin installed:<BR>
<TT>tar xzf ltools-x.y.tar.gz</TT></LI>
<LI>Compile the programs:<BR>
<TT>./build.bat</TT></LI>
<LI>Then copy <TT>lcdoc.exe</TT>, <TT>lhd.exe</TT> and <TT>lmake.exe</TT> to somewhere in your system path.</LI>
</OL>

<P CLASS="manualnav"><A HREF="instlinux.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lmake.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
