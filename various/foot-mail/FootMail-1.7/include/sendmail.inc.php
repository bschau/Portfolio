<?php
	require_once('compose.inc.php');
	require_once('inbox.inc.php');
	require_once('quotedprintable.inc.php');

function SendMail()
{
	global $hdrFrom, $page;

	$to=VarPost("to", false);
	$cc=VarPost("cc", false);
	$bcc=VarPost("bcc", false);

	if ($to===false && $cc===false && $bcc===false) {
		CollectAddresses("S");
		return;
	}

	if (($arr=FileUnserialize(MAILSESSION))===false) {
		echo "<p><br /><b>Cannot read session data on server!</b></p><br />";
		CollectAddresses("S");
		return;
	}

	if ((isset($hdrFrom)==true) && (strlen(trim($hdrFrom))>0)) {
		$retPath=trim($hdrFrom);
		$addHdr="Return-Path: ".$retPath."\r\nFrom: ".$retPath."\r\n";
	} else
		$addHdr="";

	if ($cc!==false) {
		$cc=trim($cc);
		if (strlen($cc)>0)
			$addHdr.="CC: ".$cc."\r\n";
	}

	if ($bcc!==false) {
		$bcc=trim($bcc);
		if (strlen($bcc)>0)
			$addHdr.="BCC: ".$bcc."\r\n";
	}

	$subject="=?ISO-8859-1?Q?".QuotedPrintableEncode($arr['subject'])."?=";
	$addHdr.="Content-Type: text/plain; charset=ISO-8859-1; format=flowed\r\nContent-Transfer-Encoding: 8bit\r\n";

	if (mail($to, $subject, $arr['body'], $addHdr)===false)
		echo "<p><br /><b>Failed to send mail</b></p><br />";
	else
		echo "<p><br /><b>Mail delivered</b></p><br />";

	if (isset($page)===false)
		ListInbox(0, false);
	else
		ListInbox($page, false);
}

?>
