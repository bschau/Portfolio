<?php
	define("TITLE", "lcdoc");
	define("PAGE", "prod");
	require_once('../../header.inc.php');
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">lcdoc</H1>
<P><I>lcdoc</I> is a simple javadoc for c utility. It will take special formatted comments and turn them into html documents.
<P><BR><P>
<H3 CLASS="topic">Synopsis</H3>
<P><PRE>
    Usage: lcdoc [OPTIONS] files

    [OPTIONS]
      -S style      Style sheet
      -h            This page
      -o dir        Output files to 'dir'
      -s            Add static declarations to output
      -v            Raise verbosity
</PRE>

<P><BR><P>
<H3 CLASS="topic">Usage</H3>
<P>For a quick view of what kind of files lcdoc generates, have a look at the pages generated when running lcdoc on itself: <A HREF="example/index.html">doc for v0.1</A>. The pages was generated using the <TT>-s</TT> switch to include static elements.
<P>lcdoc looks for comments starting with <TT>/**</TT> which tells lcdoc to start extracting. If the <TT>/**</TT> sequence is followed by a dash (-), like in <TT>/**-</TT> the element is ignored. Extraction stops when the comment terminator (<TT>*/</TT>) is encountered. After the terminator the element itself should follow. lcdoc tries to guess what kind of element is described. Elements include:
<UL>
<LI>Enumerations.
<LI>Structures.
<LI>Typedefs.
<LI>Variables.
<LI>Functions.
</UL>
<P>Normally, <TT>static</TT> elements are ignored. If <TT>-s</TT> is specified on the command line, <TT>static</TT> elements are included.
<P>If a valid element cannot be found, the comment is added to the top of the generated document.
<P>The comment block should follow this layout:
<PRE>
    /**
     * Description of the element.
     * @param paramA Description of parameter A.
     *    .
     *    .
     * @param paramX Description of parameter X.
     * @return What the element returns (if any).
     * @note If anything special should be highlighted.
     */
</PRE>
<P>This format is very flexible, though, and should not be followed rigidly. F.ex. it wouldn't make any sense to have a <TT>@return</TT> for enums and structures. The alias <TT>@member</TT> can be used instead of <TT>@param</TT> and really makes sense when describing enums and structures.
<P>A special directive, <TT>@type</TT>, can be used to force the element type to one of: <B>enum</B>, <B>struct</B>, <B>typedef</B>, <B>var</B> or <B>func</B>. You should seldom use this. But in the case that lcdoc wrongly guesses the element type you can force lcdoc to treat the element as one of the types. Please also drop me a mail so that I can correct lcdoc! :-)

<P>Browse the source code archive and look at the <A HREF="example/index.html">example</A> to see how it all works.

<P CLASS="manualnav"><A HREF="lmake-example.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lhd.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
