<?php
	define("TITLE", "introduction to lme");
	define("PAGE", "prod");
	require_once("../../header.inc.php");
?>
<DIV CLASS="stdpage">
<H1 CLASS="topic">lme - Introduction</H1>
<P><I>lme</I> is a macro expander. lme optionally reads one or more properties files defining various variables and then it runs through a source file and performs substitutions based on the values in the properties files or builtin commands.
<P><I>Something to substitute</I> is surrounded by the open and closing tags - these defaults to <TT>$(</TT> (open) and <TT>)</TT> (close). The tags can be redefined by the <TT>-S</TT> and <TT>-E</TT> options to <TT>lme</TT>

<P><BR><P>
<H3 CLASS="topic">Synopsis</H3>
<PRE>
    Usage: lme [OPTIONS] file

    [OPTIONS]
      -S tag        Start tag - default is $(
      -E tag        End tag - default is )
      -h            This page
      -f file       Output to file
      -o dir        Output file to 'dir'
      -p file       Properties file (can be given multiple times)
      -v            Raise verbosity
</PRE>
<P><BR><P>
<H3 CLASS="topic">Options Explained</H3>
<DL>
<DT>-S tag</DT>
<DD>Specify start tag. The default is <TT>$(</TT>. This can be set to any number of characters. The pattern should not be the same as the end tag and it should be a unique pattern not found in the source file.</DD>
<DT>-E tag</DT>
<DD>Specify end tag. The default is <TT>)</TT>. This can be set to any number of characters. The pattern should not be the same as the start tag and it should be a somewhat unique pattern.</DD>
<DT>-h</DT>
<DD>Show help page.</DD>
<DT>-f file</DT>
<DD>Output to file. See below.</DD>
<DT>-o dir</DT>
<DD>Output to directory. See below.</DD>
<DT>-p file</DT>
<DD>Read this properties file before parsing the source file. Multiple properties files can be given and keys found in later files will overwrite keys found in earlier files.</DD>
<DT>-v</DT>
<DD>Raise verbosity.</DD>
</DL>

<P><BR><P>
<H3 CLASS="topic">Output</H3>
<P>If neither the <TT>-f file</TT> or <TT>-o dir</TT> switch is given output will go to the current directory. <B>Note!</B> If you run <TT>lme</TT> in the same directory as the source file and you don'r specify either the <TT>-f file</TT> or <TT>-o dir</TT> switch your source file will be overwritten. You have been warned!
<P>If you specify the <TT>-f file</TT> switch you must specify a full path to the output file, such as <TT>/home/bsc/main.java</TT>.
<P>If you specify the <TT>-o dir</TT> file and the argument to <TT>-o</TT> is <TT>/home/bsc</TT> and the source file is <TT>src/main.java</TT> the output file will be placed in <TT>/home/bsc/main.java</TT>.

<P CLASS="manualnav"><A HREF="lhd.php">&lt;&lt;</A> .:. <A HREF="manual.php">UP</A> .:. <A HREF="lme-properties.php">&gt;&gt;</A>
</DIV>
</BODY>
</HTML>
