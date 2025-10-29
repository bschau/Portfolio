<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//WAPFORUM//DTD XHTML Mobile 1.0//EN" "http://www.wapforum.org/DTD/xhtml-mobile10.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<title>FootMail - Configure</title>
	</head>

	<body>
		<p><a href="configure.php" accesskey="4">Back</a>&nbsp;&nbsp;<a href="index.html" accesskey="2">Menu</a></p>
<?php

function VarPost($var, $default)
{
	if ((isset($_POST[$var])==true) && (strlen(trim($_POST[$var]))>0))
		return trim($_POST[$var]);

	return $default;
}

function FileWrite($name, $src)
{
	if (($fp=@fopen($name, 'w+'))===false)
		return false;

	$ret=@fwrite($fp, $src);
	@fclose($fp);

	return ($ret===false) ? false : true;
}

function FixUp()
{
	global $mbServer, $mbPort, $mbServerUser, $mbServerPass;
	global $hdrFrom, $timeZone, $signature;
	global $pageSize;
	global $replyHdrs, $replyDefault, $indentChar;

	if (isset($mbServer) && $mbServer!==false)
		$mbServer=trim($mbServer);
	else
		$mbServer="localhost";

	if (isset($mbPort) && $mbPort!==false)
		$mbPort=trim($mbPort);
	else
		$mbPort="110";

	if (isset($mbServerUser) && $mbServerUser!==false)
		$mbServerUser=trim($mbServerUser);
	else
		$mbServerUser="";

	if (isset($mbServerPass) && $mbServerPass!==false)
		$mbServerPass=trim($mbServerPass);
	else
		$mbServerPass="";

	if (isset($hdrFrom) && $hdrFrom!==false)
		$hdrFrom=trim($hdrFrom);
	else
		$hdrFrom="";

	if (isset($timeZone) && $timeZone!==false)
		$timeZone=intval(trim($timeZone));
	else
		$timeZone=2;

	if (isset($signature) && $signature!==false)
		$signature=trim($signature);
	else
		$signature="";

	if (isset($pageSize) && $pageSize!==false)
		$pageSize=intval(trim($pageSize));
	else
		$pageSize=5;

	if (isset($replyHdrs) && $replyHdrs!==false) {
		$replyHdrs=str_replace("\t", " ", $replyHdrs);
		$replyHdrs=eregi_replace(" +", " ", $replyHdrs);
		$replyHdrs=strtolower(trim($replyHdrs));
	} else
		$replyHdrs="re: sv:";

	if (isset($replyDefault)===false || $replyDefault===false)
		$replyDefault="Re: ";

	if (isset($indentChar)===false || $indentChar===false)
		$indent="> ";
}

function LoadVars()
{
	global $mbServer, $mbPort, $mbServerUser, $mbServerPass;
	global $hdrFrom, $timeZone, $signature;
	global $pageSize;
	global $replyHdrs, $replyDefault, $indentChar;
	global $chkDeleted;

	$errs="";
	if (($mbServer=VarPost('mbServer', false))===false)
		$errs.="<b>You must enter a server name or IP address.</b>\n";

	$mbPort=VarPost('mbPort', false);
	if (($mbServerUser=VarPost('mbServerUser', false))===false)
		$errs.="<b>You must enter a user name.</b>\n";

	if (($mbServerPass=VarPost('mbServerPass', false))===false)
		$errs.="<b>You must enter a password.</b>\n";

	if (($hdrFrom=VarPost('hdrFrom', false))===false)
		$errs.="<b>You must enter a valid email address.</b>\n";

	$timeZone=VarPost('timeZone', false);
	$signature=VarPost('signature', false);
	$pageSize=VarPost('pageSize', false);
	$replyHdrs=VarPost('replyHdrs', false);
	$replyDefault=VarPost('replyDefault', false);
	$indentChar=VarPost('indentChar', false);
	$chkDeleted=VarPost('chkDeleted', false);

	if ($chkDeleted==='on')
		$chkDeleted=true;
	else
		$chkDeleted=false;

	return $errs;
}

function SaveConfig()
{
	global $TMP;
	global $mbServer, $mbPort, $mbServerUser, $mbServerPass;
	global $hdrFrom, $timeZone, $signature;
	global $pageSize;
	global $replyHdrs, $replyDefault, $indentChar;
	global $chkDeleted;

	FixUp();

	if (strlen($signature)>0)
		FileWrite($TMP."/includeSignature", "yes");
	else
		@unlink($TMP."/includeSignature");

	$fc="<?php\n";
	$fc.="    \$mbServer=\"".$mbServer."\";\n";
	$fc.="    \$mbPort=\"".$mbPort."\";\n";
	$fc.="    \$mbServerUser=\"".$mbServerUser."\";\n";
	$fc.="    \$mbServerPass=\"".$mbServerPass."\";\n";
	$fc.="    \$hdrFrom=\"".$hdrFrom."\";\n";
	$fc.="    \$timeZone=".$timeZone.";\n";
	$fc.="    \$signature=\"".$signature."\";\n";
	$fc.="    \$pageSize=".$pageSize.";\n";
	$fc.="    \$replyHdrs=\"".$replyHdrs."\";\n";
	$fc.="    \$replyDefault=\"".$replyDefault."\";\n";
	$fc.="    \$indentChar=\"".$indentChar."\";\n";
	$fc.="    \$chkDeleted=";
	if ($chkDeleted===true)
		$fc.="true;\n";
	else
		$fc.="false;\n";

	$fc.="?>\n";

	return FileWrite('config.inc.php', $fc);
}

function PingPong($sck, $src)
{
	if (@fwrite($sck, $src."\r\n")!==false) {
		if (($ret=@fgets($sck, 8192))!==false) {
			if (strncasecmp("+OK", $ret, 3)==0)
				return trim($ret);
		}
	}

	fclose($sck);
	return false;
}

function POP3Test()
{
	global $mbServer, $mbPort, $mbServerUser, $mbServerPass;

	echo "Opening connection to ".$mbServer." on port ".$mbPort."\n";
	if (($sck=@fsockopen($mbServer, $mbPort))===false)
		return false;

	echo "Reading banner\n";
	if (($ret=@fgets($sck, 8192))===false)
		return false;

	if (strncasecmp("+OK", $ret, 3)!=0) {
		echo "Server is not ready\n";
		return false;
	}

	echo "Sending USER\n";
	if (PingPong($sck, "USER ".$mbServerUser)===false)
		return false;

	echo "Sending PASS\n";
	if (PingPong($sck, "PASS ".$mbServerPass)===false)
		return false;

	echo "Sending STAT\n";
	if (PingPong($sck, "STAT")===false)
		return false;

	return true;
}

	$TMP="tmp";
	if (@is_dir($TMP)===false) {
		if (@file_exists($TMP)===true) {
			echo "<p><b>TMP exists but is not a directory</b></p></body></html>";
			exit;
		}

		if (@mkdir($TMP)===false) {
			echo "<p><b>Failed to create TMP</b></p></body></html>";
			exit;
		}
	}

	$showForm=false;
	$configure=VarPost('configure', false);
	$tryConnect=VarPost('tryConnect', false);
	if ($configure!==false) {
		$errs=trim(LoadVars());
		if (strlen($errs)==0) {
			if (SaveConfig()==false) {
				echo "<p><br /><p><b>Failed to save configuration!</b></p><br /></p>";
				$showForm=true;
			} else
				echo "<p><br /><p><b>Configuration saved.</b></p><br /></p>";
		} else {
			$showForm=true;
			echo "<p>".nl2br($errs)."</p>";
		}

	} else if ($configure===false && $tryConnect!==false) {
		$errs=trim(LoadVars());
		if (strlen($errs)==0) {
			if (SaveConfig()==true) {
				echo "<b>Connection Progress:</b>";
				echo "<pre>";
				if (($err=POP3Test())===false)
					echo "... FAILED!";
				else
					echo "OK";

				echo "</pre>";
			} else
				echo "<p><br /><p><b>Failed to save configuration!</b></p><br /></p>";
		}
	} else {
		// Server Settings
		$mbServer=false;
		$mbPort=false;
		$mbServerUser=false;
		$mbServerPass=false;

		// Personal Settings
		$hdrFrom=false;
		$timeZone=false;
		$signature="This mail has been sent from my Mobile Phone using FootMail.";

		// Inbox Settings
		$pageSize=false;

		// Reply Settings
		$replyHdrs=false;
		$replyDefault=false;
		$indentChar=false;

		// Misc Settings
		$chkDeleted=false;

		if (is_file('config.inc.php')==true)
			require_once('config.inc.php');

		FixUp();
		$showForm=true;
	}

	if ($showForm) {
		echo "<form method=\"post\" action=\"configure.php\">";

		echo "<h1>Server Settings</h1>";
		echo "<p><b>POP3 Server</b><br /><input type=\"text\" size=\"20\" maxlength=\"128\" name=\"mbServer\" value=\"".$mbServer."\" /></p>";
		echo "<p><b>POP3 Server Port</b><br /><input type=\"text\" size=\"5\" maxlength=\"5\" name=\"mbPort\" value=\"".$mbPort."\" /></p>";
		echo "<p><b>POP3 User Name</b><br /><input type=\"text\" size=\"20\" maxlength=\"128\" name=\"mbServerUser\" value=\"".$mbServerUser."\" /></p>";
		echo "<p><b>POP3 Password</b><br /><input type=\"password\" size=\"20\" maxlength=\"128\" name=\"mbServerPass\" value=\"".$mbServerPass."\" /></p>";

		echo "<h1>Personal Settings</h1>";
		echo "<p><b>Email Address</b><br /><input type=\"text\" size=\"20\" maxlength=\"128\" name=\"hdrFrom\" value=\"".$hdrFrom."\" /></p>";
		echo "<p><b>Time Zone</b><br /><input type=\"text\" size=\"5\" maxlength=\"5\" name=\"timeZone\" value=\"".$timeZone."\" /></p>";
		echo "<p><b>Signature</b><br /><textarea cols=\"60\" rows=\"5\" name=\"signature\">".$signature."</textarea></p>";

		echo "<h1>Inbox Settings</h1>";
		echo "<p><b>Page Size</b><br /><input type=\"text\" size=\"5\" maxlength=\"5\" name=\"pageSize\" value=\"".$pageSize."\" /></p>";

		echo "<h1>Reply Settings</h1>";
		echo "<p><b>Remove Reply Prefixes</b><br /><input type=\"text\" size=\"20\" maxlength=\"128\" name=\"replyHdrs\" value=\"".$replyHdrs."\" /></p>";
		echo "<p><b>Default Reply Prefix</b><br /><input type=\"text\" size=\"20\" maxlength=\"128\" name=\"replyDefault\" value=\"".$replyDefault."\" /></p>";
		echo "<p><b>Indent String</b><br /><input type=\"text\" size=\"20\" maxlength=\"128\" name=\"indentChar\" value=\"".$indentChar."\" /></p>";

		echo "<h1>Shortcut Settings</h1>";
		echo "<p><b>Auto-check <i>Deleted</i> checkbox</b><br /><input type=\"checkbox\" name=\"chkDeleted\"";
		if ($chkDeleted===true)
			echo " checked";
		echo " /></p>";

		echo "<input type=\"submit\" name=\"configure\" value=\"Configure\" />&nbsp;&nbsp;<input type=\"submit\" name=\"tryConnect\" value=\"Try Connect\" /></form>";
	}
?>
		<p><a href="configure.php" accesskey="4">Back</a>&nbsp;&nbsp;<a href="index.html" accesskey="2">Menu</a></p>
	</body>
</html>
