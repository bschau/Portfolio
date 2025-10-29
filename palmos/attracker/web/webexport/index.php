<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<HTML>
<HEAD><TITLE>Web Export</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=ISO-8859-1">
<LINK REL="STYLESHEET" TYPE="text/css" HREF="/gz/online/stylesheet.css">
</HEAD>
<BODY>
<H1>@Tracker - Web Export</H1>
<?php
	$dateFormat="ymddash";
	$timeFormat="hmcolon";
	$lineFeedType="w";
	$outputFormat="excel";
	$keepWhite="off";

	if (isset($_COOKIE['SchauComWebExport']) && strlen(trim($_COOKIE['SchauComWebExport']))>0) {
		$cArr=explode("-", $_COOKIE['SchauComWebExport']);
		if (isset($cArr[0]))
			$dateFormat=$cArr[0];

		if (isset($cArr[1]))
			$timeFormat=$cArr[1];

		if (isset($cArr[2]))
			$lineFeedType=$cArr[2];

		if (isset($cArr[3]))
			$outputFormat=$cArr[3];

		if (isset($cArr[4]))
			$keepWhite=$cArr[4];
	}
?>
<FORM ENCTYPE="multipart/form-data" METHOD="post" ACTION="export.php">
<P><INPUT TYPE="HIDDEN" NAME="MAX_FILE_SIZE" VALUE="70000">
<TABLE>
<TR>
	<TD ALIGN="RIGHT">Archive File:</TD>
	<TD><INPUT TYPE="FILE" NAME="archiveFile" SIZE="30"></TD>
</TR>
<TR>
	<TD ALIGN="RIGHT">Date Format:</TD>
	<TD><SELECT NAME="dateFormat">
<?php
	$dateArr=array("ymdslash" => "yyyy/mm/dd", "ymddot" => "yyyy.mm.dd", "ymddash" => "yyyy-mm-dd", "mdyslash" => "mm/dd/yyyy", "mdydot" => "mm.dd.yyyy", "mdydash" => "mm-dd-yyyy", "dmyslash" => "dd/mm/yyyy", "dmydot" => "dd.mm.yyyy", "dmydash" => "dd-mm-yyyy");

	while (list($format, $string)=each($dateArr)) {
		echo "<OPTION VALUE=\"".$format."\"";
		if ($format==$dateFormat)
			echo " SELECTED";

		echo ">".$string."</OPTION>";
	}
?></SELECT></TD>
</TR>
	
<TR>
	<TD ALIGN="RIGHT">Time Format:</TD>
	<TD><SELECT NAME="timeFormat">
<?php
	$timeArr=array("hmcolon" => "hh:mm", "hmdot" => "hh.mm", "hmcomma" => "hh,mm");

	while (list($format, $string)=each($timeArr)) {
		echo "<OPTION VALUE=\"".$format."\"";
		if ($format==$timeFormat)
			echo " SELECTED";

		echo ">".$string."</OPTION>";
	}
?></SELECT></TD>
</TR>

<TR>
	<TD ALIGN="RIGHT">Linefeed Type:</TD>
	<TD><SELECT NAME="lineFeedType">
<?php
	$lfArr=array("w" => "Windows", "u" => "Unix/Linux", "m" => "Macintosh");

	while (list($format, $string)=each($lfArr)) {
		echo "<OPTION VALUE=\"".$format."\"";
		if ($format==$lineFeedType)
			echo " SELECTED";

		echo ">".$string."</OPTION>";
	}
?></SELECT></TD>
</TR>

<TR>
	<TD ALIGN="RIGHT">Output Format:</TD>
	<TD><SELECT NAME="outputFormat">
<?php
	$outputArr=array("excel" => "Microsoft Excel CSV", "stdcsv" => "Standard CSV", "php" => "PHP", "debug" => "Debug");

	while (list($format, $string)=each($outputArr)) {
		echo "<OPTION VALUE=\"".$format."\"";
		if ($format==$lineFeedType)
			echo " SELECTED";

		echo ">".$string."</OPTION>";
	}
?></SELECT></TD>
</TR>

<TR>
	<TD ALIGN="RIGHT">Keep Whitespaces:</TD>
	<TD><INPUT TYPE="CHECKBOX" NAME="keepWhite"<?php if (strcasecmp($keepWhite, "on")==0) echo " CHECKED"; ?>></TD>
</TR>

<TR>
	<TD COLSPAN="2" ALIGN="CENTER"><INPUT TYPE="SUBMIT" VALUE="Export"></TD>
</TR>
</TABLE>
</FORM>
<P><BR><P><A HREF="/index.html">Home</A>&nbsp;&nbsp;<A HREF="instructions.html">Instructions</A>
</BODY>
</HTML>
