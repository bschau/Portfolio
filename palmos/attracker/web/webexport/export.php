<?php

function VarPost($var, $default)
{
	if (isset($_POST[$var]) && strlen(trim($_POST[$var]))>0)
		return trim($_POST[$var]);

	return $default;
}

function CleanUp()
{
	global $tmpFile;

	if ($tmpFile!==false)
		@unlink($tmpFile);
}

	$dateFormat=VarPost('dateFormat', 'ymddash');
	$timeFormat=VarPost('timeFormat', 'hmcolon');
	$lineFeedType=VarPost('lineFeedType', 'w');
	$outputFormat=VarPost('outputFormat', 'excel');
	$keepWhite=VarPost('keepWhite', "off");

	$cVal=$dateFormat."-".$timeFormat."-".$lineFeedType."-".$outputFormat."-".$keepWhite;

	setcookie('SchauComWebExport', $cVal, time()*60*60*24*90);

	$errStr=false;
	if (isset($_FILES['archiveFile']['tmp_name']) && is_uploaded_file($_FILES['archiveFile']['tmp_name'])) {
		$cmd="/usr/local/bin/tttxexport -D ".$dateFormat." -L ".$lineFeedType." -O ".$outputFormat." -T ".$timeFormat;
		if (strcasecmp($keepWhite, "on")==0)
			$cmd.=" -W";
		
		$tmpFile=false;
		register_shutdown_function('CleanUp');
		$tmpFile=@tempnam("/tmp", "SCWE");
		if ($tmpFile!==false) {
			$cmd.=" -o ".$tmpFile." ".$_FILES['archiveFile']['tmp_name'];

			@system(escapeshellcmd($cmd), $retCode);
			if ($retCode==0 && (($fs=filesize($tmpFile))>0)) {
				switch (strtolower($outputFormat)) {
				case 'excel':
				case 'stdcsv':
					$suffix=".csv";
					break;
				case 'php':
					$suffix=".php";
					break;
				default:
					$suffix=".txt";
					break;
				}

				$bn=basename($_FILES['archiveFile']['name']);
				if (($pos=strrpos($bn, '.'))===false)
					$bn.=$suffix;
				else
					$bn=substr($bn, 0, $pos).$suffix;

				header("Content-Description: File Transfer");
				header("Content-Type: application/octet-stream");
				header("Content-Length: ".$fs);
				header("Content-Disposition: attachment; filename=\"".str_replace(" ", "_", $bn)."\"");
				readfile($tmpFile);
				exit;
			} else
				$errStr="Invalid filename given (or internal tttxexport error).<BR>Error code returned from the program is ".$retCode.".";
		} else
			$errStr="Cannot create temporary file.";
	} else
		$errStr="No file given or file upload error.";
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<HTML>
<HEAD><TITLE>Web Export</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=ISO-8859-1">
<LINK REL="STYLESHEET" TYPE="text/css" HREF="/gz/online/stylesheet.css">
</HEAD>
<BODY>
<H1>@Tracker - Web Export</H1>
<P STYLE="color: red">Export Error: <?php echo $errStr; ?>

<P><BR><P><A HREF="/index.html">Home</A>
</BODY>
</HTML>
