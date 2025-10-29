<?php
	define("MGW", "mgw.schau.dk");

/*******************************************************************************
*
*	mgwconnect
*
*	Open connection to mgw host.
*
*	Input:
*		cmd - command to send to remote.
*	Output:
*		s: true.
*		f: false.
*/
function mgwconnect($cmd)
{
	global $sck;

	$ret=false;
	$port=getservbyname("qmta", "tcp");
	$sck=fsockopen(MGW, $port);
	if ($sck==false)
		echo "<P>Cannot open connection to ".MGW." on port $port";
	else {
		$err=fputs($sck, trim($cmd)."\n");
		if ($err==false) 
			echo "<P>Cannot write to socket";
		else
			$ret=true;
	}

	return $ret;
}

	$msg="";
	if (isset($cmd)) {
		if (strcmp($cmd, "ignore")) {
			$err=mgwconnect($cmd);
			if ($err==true) {
				while (!feof($sck)) {
					$line=trim(fgets($sck, 1024));
					$msg.=$line."<BR>";
				}
			}
		}
	}

	$err=mgwconnect("time");
	$rawdate="";
	if ($err==true) {
		while (!feof($sck)) {
			$line=trim(fgets($sck, 1024));
			if (strlen($rawdate)==0)
				$rawdate=$line;
		}
	}

	$brokendown=explode(" ", $rawdate);
	$sysdate=$brokendown[0];
	$systime=$brokendown[1];
?>
<HTML>
<HEAD><TITLE>QMTA</TITLE>
<STYLE TYPE="text/css">
body,td {font-family:verdana,geneva,arial,helvetica;font-size:10px}
h1 {font-family:verdana,geneva,arial,helvetica;font-size:20px}
p {font-family:verdana,geneva,arial,helvetica;font-size:14px}
</STYLE>
</HEAD>
<BODY BGCOLOR="#ffffff"><H1>QMTA</H1><P>
<TABLE><TR>
<TD>Spoolk&oslash;:
<BR><TEXTAREA ROWS="16" COLS="30" WRAP="off">
<?php
	$sweep=0;
	$cntl=0;

	$err=mgwconnect("listqueue");
	if ($err==true) {
		while (!feof($sck)) {
			$line=trim(fgets($sck, 1024));
			if (strlen($line)>1) {
				if ($line[7]==':') {
					if ($line[0]=='s')
						$sweep++;
					else
						$cntl++;
				} else
					echo "&nbsp; ";

				echo $line."\n";
			}
		}
	}
?>
</TEXTAREA>
</TD>
<TD>&nbsp; &nbsp; &nbsp;</TD>
<TD VALIGN="TOP">Processer:
<BR><TEXTAREA ROWS="16" COLS="30" WRAP="off">
<?php
	$err=mgwconnect("status");
	if ($err==true) {
		while (!feof($sck)) {
			$line=trim(fgets($sck, 1024));
			echo $line."\n";
		}
	}
?>
</TEXTAREA>
</TD>
<TD>&nbsp; &nbsp; &nbsp;</TD>
<TD VALIGN="TOP"><BR>
QMTAs ide om ...<BR>
<TABLE>
<TR><TD>... dato:</TD><TD>&nbsp;</TD><TD><FONT COLOR="blue"><?php echo $sysdate; ?></FONT></TD></TR>
<TR><TD>... klokkesl&aelig;t:</TD><TD>&nbsp;</TD><TD><FONT COLOR="blue"><?php echo $systime; ?></FONT></TD></TR>
</TABLE>
<P><BR>
<TABLE>
<TR><TD>Kontrolfiler:</TD><TD>&nbsp;</TD><TD><FONT COLOR="blue"><?php echo $cntl; ?></FONT></TD></TR>
<TR><TD>Afventer scanning:</TD><TD>&nbsp;</TD><TD><FONT COLOR="blue"><?php echo $sweep; ?></FONT></TD></TR>
<TR><TD><B>Total:</B></TD><TD>&nbsp;</TD><TD><FONT COLOR="blue"><B><?php echo $cntl+$sweep; ?></B></FONT></TD></TR>
</TABLE>
<P><BR>
<FORM METHOD="POST" ACTION="qmta.php">
<INPUT TYPE="RADIO" NAME="cmd" VALUE="ignore" CHECKED>Opdater billede<BR>
<INPUT TYPE="RADIO" NAME="cmd" VALUE="start">Start QMTA<BR>
<INPUT TYPE="RADIO" NAME="cmd" VALUE="stop">Stop QMTA<BR>
<INPUT TYPE="RADIO" NAME="cmd" VALUE="restart">Genstart QMTA<BR>
<P><INPUT TYPE="SUBMIT" VALUE="Udf&oslash;r">
</FORM>
<BR><FONT COLOR="red">
<?php
	if (strlen($msg)>0)
		echo $msg;
?>
</TD>
</TR></TABLE>
</BODY>
</HTML>
