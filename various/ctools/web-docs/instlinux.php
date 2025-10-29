<?php
	define("TITLE", "installation on Linux");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">Installation on Linux</H1>
<P>To install <I>ltools</I> on Linux follow these steps:
<OL>
<LI>If you don't have <A HREF="http://gcc.gnu.org/">gcc</A> installed please install it. Normally it is included in a <B>Development</B> section in your chosen distribution - so pick it up from there.</LI>
<LI>Unpack the <TT>ltools-x.y.tar.gz</TT> archive:<BR>
<TT>tar xzf ltools-x.y.tar.gz</TT></LI>
<LI>Become <B>root</B>:<BR>
<TT>su -</TT></LI>
<LI>Compile and install the programs:<BR>
<TT>./build.sh</TT><BR>... answer the relevant questions</LI>
</OL>
<P>And that is it!

<P CLASS="manualnav"><A HREF="manual.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="instwin.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
