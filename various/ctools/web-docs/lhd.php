<?php
	define("TITLE", "lhd");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">lhd</H1>
<P>lhd is a lightweight hex dump utility much like <I>xxd</I>. lhd outputs offset, hex dump and ascii dump given zero or more input ranges.

<P><BR><P>
<H3 CLASS="topic">Synopsis</H3>
<PRE>
    Usage: lhd file [ranges]

    Ranges can be given as:

      r1:  or
      r1       Show from 'r1' to the end of the file
      :r2  or
      +r2      Show from the start of the file to 'r2'
      r1:r2    Show from 'r1' to 'r2'
      r1+r2    Show from 'r1' and 'r2' bytes onwards

    Multiple ranges must be given - they should be separated by whitespaces
    Values can be prefixed with:

      0x1234 or
      x1234    Hexadecimal value
      01234    Octal value
      %1010    Binary value

    No prefix means plain old decimal system
</PRE>

<P><BR><P>
<H3 CLASS="topic">Examples</H3>
<P>Examples (run on lhd itself on linux):
<UL>
<LI><TT>lhd lhd</TT><BR><PRE>
    0000000: 7f45 4c46 0101 0100 0000 0000 0000 0000  .ELF............
    0000010: 0200 0300 0100 0000 b085 0408 3400 0000  ............4...
    0000020: d41b 0000 0000 0000 3400 2000 0700 2800  ........4. ...(.
    0000030: 2200 1f00 0600 0000 3400 0000 3480 0408  ".......4...4...
    0000040: 3480 0408 e000 0000 e000 0000 0500 0000  4...............
</PRE><BR>(Output truncated)
<LI><TT>lhd lhd 0:10</TT><BR><PRE>
    0000000: 7f45 4c46 0101 0100 0000                 .ELF......
</PRE><BR>(Show first ten bytes - same as <TT>lhd lhd +10</TT> and <TT>lhd lhd :10</TT>)
<LI><TT>lhd lhd 010:020 0x10+16</TT><BR><PRE>
    0000008: 0000 0000 0000 0000                      ........
    0000010: 0200 0300 0100 0000 b085 0408 3400 0000  ............4...
</PRE><BR>(Show two ranges ...)
</UL>

<P CLASS="manualnav"><A HREF="lcdoc.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lme.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
