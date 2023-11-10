<?
	define("MGW", "mgw.schau.dk");

	echo "<HTML><HEAD><TITLE>Output</TITLE></HEAD>";
	echo "<BODY BGCOLOR=\"#FFFFFF\">";
	echo "<P>";

	$foot="</BODY></HTML>";

	if (!isset($command)) {
		echo "<P>Ooops, no command given".$foot;
		exit;
	}

	// open connection
	$port=getservbyname("qmta", "tcp");
	$sck=fsockopen(MGW, $port);
	if ($sck==false) {
		echo "<P>Cannot open connection to ".MGW." on port $port".$foot;
		exit;
	}

	// write command
	$err=fputs($sck, trim($command)."\n");
	if ($err==false) {
		echo "<P>Cannot write to socket".$foot;
		exit;
	}

	// read back answer
	while (!feof($sck)) {
		$line=trim(fgets($sck, 1024));
		echo $line."<BR>";
	}

	echo $foot;

	// done ...
?>
