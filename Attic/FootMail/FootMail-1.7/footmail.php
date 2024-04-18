<?php

function Redirect($page)
{
	header("Location: http://".$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF']).$page);
	exit;
}

function PageHeader()
{
	echo <<<EOF
<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//WAPFORUM//DTD XHTML Mobile 1.0//EN" "http://www.wapforum.org/DTD/xhtml-mobile10.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<title>FootMail</title>
</head>
<body>
EOF;
}

function PageFooter()
{
	echo <<<EOF
</body>
</html>
EOF;
}

function VarGet($var, $default)
{
	if ((isset($_GET[$var])==true) && (strlen(trim($_GET[$var]))>0))
		return trim($_GET[$var]);

	return $default;
}

function VarPost($var, $default)
{
	if ((isset($_POST[$var])==true) && (strlen(trim($_POST[$var]))>0))
		return trim($_POST[$var]);

	return $default;
}

function VarGetPost($var, $default)
{
	if ((isset($_GET[$var])==true) && (strlen(trim($_GET[$var]))>0))
		return urldecode($_GET[$var]);
	else if ((isset($_POST[$var])==true) && (strlen(trim($_POST[$var]))>0))
		return trim($_POST[$var]);

	return $default;
}

function OpenIfMail()
{
	global $mail, $mBox;

	if ($mail!==false) {
		if (($mBox=MBOpen())!==false)
			return;
		else
			echo "<p>Cannot open connection to Mailbox</p>";
	} else
		echo "<p>No such mail</p>";

	PageFooter();
	exit;
}

function FileUnserialize($name)
{
	if (($res=@file_get_contents($name))===false)
		return false;

	return unserialize($res);
}

function FileWrite($name, $src)
{
	if (($fp=@fopen($name, 'w+'))===false)
		return false;

	$ret=@fwrite($fp, $src);
	@fclose($fp);

	return ($ret===false) ? false : true;
}

	$chkDeleted=false;

	require_once('config.inc.php');
	require_once('include/mailbox.inc.php');

	header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
	header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
	header("Cache-Control: no-store, no-cache, must-revalidate");
	header("Cache-Control: post-check=0, pre-check=0", false);
	header("Pragma: no-cache");

	$cmd=VarGetPost('c', 'I');
	$filter=VarGetPost('F', false);
	if ($filter!==false && $filter=='*')
		$filter=false;

	$page=VarGetPost('p', 0);
	$mail=VarGetPost('m', false);
	$segment=VarGetPost('s', false);
	$submitType=VarGetPost('submitType', false);

	if ($submitType!==false) {
		if ($submitType=='Delete') {
			require_once('include/maintenance.inc.php');

			MaintenanceDelete();
			readfile('deleted.html');
			exit;
		}
	}

	PageHeader();
	$TMP="tmp";
	if (@is_dir($TMP)===false) {
		if (@file_exists($TMP)===true) {
			echo "<p><b>TMP exists but is not a directory</b></p>";
			PageFooter();
			exit;
		}

		if (@mkdir($TMP)===false) {
			echo "<p><b>Failed to create TMP</b></p>";
			PageFooter();
			exit;
		}
	}

	$mBox=false;
	define("MAILSESSION", $TMP."/outgoing.inc.php");

	switch ($cmd) {
	case 'A':	// View attachment
		require_once('include/attachment.inc.php');

		OpenIfMail();
		$hdr="<a href=\"footmail.php?c=V&p=".$page."&m=".$mail."\">Back</a>";
		echo "<p>".$hdr."</p>";
		ViewAttachment();
		echo "<p>".$hdr."</p>";

		break;

	case 'B':	// View body
		OpenIfMail();
		$hdr="<a href=\"footmail.php?c=V&p=".$page."&m=".$mail."\">Back</a>";

		echo "<p>".$hdr."</p>";
		echo "<pre>".htmlentities(@imap_body($mBox, $mail))."</pre>";
		echo "<p>".$hdr."</p>";

		break;

	case 'C':	// Compose
		require_once('include/compose.inc.php');

		$mail=false;
		$segment=false;
		$mBox=false;
		$page=0;
		Compose("C1", false, false, false);
		break;

	case 'C1':	// Compose - Collect addresses
		require_once('include/compose.inc.php');

		$page=0;
		ComposeCollectMail("C1");
		break;

	case 'F':	// Forward
		require_once('include/compose.inc.php');

		OpenIfMail();
		if (($msg=MBMessageInfo($mBox, $mail))==false) {
			echo "<p>Invalid message</p>";
			break;
		}

		$subject=ComposeTrimSubject($msg['subject']);

		$msg=new message_components($mBox);
		$msg->fetch_structure($mail);

		if (($body=MBGetBody($mBox, $mail, $msg->pid[$mail][$segment], $msg->encoding[$mail][$segment]))==false) {
			echo "<p>Invalid part id</p>";
			break;
		}

		if ((isset($indentChar)==true) && (strlen(trim($indentChar))>0)) {
			$body=str_replace("\n", $indentChar, trim($body));
			$body=$indentChar.$body;
		}

		Compose("C1", $subject, $body);
		break;

	case 'H':	// View mail headers
		OpenIfMail();
		$hdr="<a href=\"footmail.php?c=V&p=".$page."&m=".$mail."\">Back</a>";

		echo "<p>".$hdr."</p>";
		echo "<pre>".htmlentities(@imap_fetchheader($mBox, $mail))."</pre>";
		echo "<p>".$hdr."</p>";

		break;

	case 'I':	// List Inbox
		require_once('include/inbox.inc.php');

		ListInbox($page, $filter);
		break;

	case 'R':	// Reply
		require_once('include/compose.inc.php');

		OpenIfMail();
		if (($msg=MBMessageInfo($mBox, $mail))==false) {
			echo "<p>Invalid message</p>";
			break;
		}

		$subject=ComposeTrimSubject($msg['subject']);

		$msg=new message_components($mBox);
		$msg->fetch_structure($mail);

		if (($body=MBGetBody($mBox, $mail, $msg->pid[$mail][$segment], $msg->encoding[$mail][$segment]))==false) {
			echo "<p>Invalid part id</p>";
			break;
		}

		if ((isset($indentChar)==true) && (strlen(trim($indentChar))>0)) {
			$body=str_replace("\n", $indentChar, trim($body));
			$body=$indentChar.$body;
		}

		Compose("R1", $subject, $body);
		break;

	case 'R1':	// Reply - Collect addresses
		require_once('include/compose.inc.php');

		OpenIfMail();
		$page=0;
		ComposeCollectMail("R1");
		break;

	case 'S':	// Send mail
		require_once('include/sendmail.inc.php');

		SendMail();
		break;

	case 'V':	// View mail
		require_once('include/viewmail.inc.php');

		OpenIfMail();
		ViewMail();
		break;

	default:
		echo "<p>Unknown command</p>";
		break;
	}

	MBClose();
	PageFooter();
?>
