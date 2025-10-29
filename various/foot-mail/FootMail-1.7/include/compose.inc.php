<?php
	require_once('messagecomponents.inc.php');

function SetupMail()
{
	if (is_file(MAILSESSION)===true) {
		if (@unlink(MAILSESSION)===false) {
			echo "<p>Cannot create session</p>";
			return false;
		}
	}

	return true;
}

function ComposeTrimSubject($sub)
{
	global $replyHdrs, $replyDefault;

	if (isset($replyHdrs)!==false) {
		$r=trim($replyHdrs);
		if (strlen($r)>0) {
			$arr=explode(" ", $replyHdrs);
			do {
				$sub=ltrim($sub);
				$foundOne=false;

				reset($arr);
				while (list($k, $v)=each($arr)) {
					$vLen=strlen($v);
					if (strncasecmp($v, $sub, $vLen)==0) {
						$sub=substr($sub, $vLen);
						$foundOne=true;
						break;
					}
				}
			} while ($foundOne);
		}
	}

	$sub=trim($sub);
	if (isset($replyDefault)==true) {
		$r=trim($replyDefault);
		if (strlen($r)>0)
			$sub=$replyDefault." ".$sub;
	}

	return $sub;
}

function Compose($nextCmd, $subject, $body)
{
	global $mail, $page, $TMP;

	if (SetupMail()===false)
		return;

	$hdr="<a href=\"footmail.php?p=".$page."\" accesskey=\"1\">Inbox</a>&nbsp;&nbsp;<a href=\"index.html\" accesskey=\"2\">Menu</a>";

	echo "<p>".$hdr."</p><form method=\"post\" action=\"footmail.php\"><input type=\"hidden\" name=\"c\" value=\"".$nextCmd."\" /><input type=\"hidden\" name=\"p\" value=\"".$page."\" />";
	if ($mail!==false)
		echo "<input type=\"hidden\" name=\"m\" value=\"".$mail."\" />";

	if (@file_exists($TMP."/includeSignature")===true)
		$incSig=" checked";
	else
		$incSig="";

	echo "<p><input type=\"checkbox\" name=\"includeSignature\" value=\"yes\" ".$incSig." /> Include Signature</p>";
	if ($subject===false)
		$subject="";

	echo "<p><b>Subject:</b></p>";
	echo "<input type=\"text\" maxlength=\"200\" size=\"20\" name=\"subject\" accesskey=\"5\" value=\"".htmlentities($subject)."\" />";

	echo "<br /><textarea cols=\"20\" rows=\"10\" name=\"body\" accesskey=\"7\">";

	if ($body===false)
		$body="";

	echo htmlentities($body);
	echo "</textarea>";
	echo "<br /><input type=\"submit\" value=\"Next &gt;&gt;\" accesskey=\"6\" /></form>";
	echo "<p>".$hdr."</p>";
}

function StoreSession($s, $b)
{
	$m=array();
	if ($s===false || strlen(trim($s))==0)
		$m['subject']="";
	else
		$m['subject']=trim($s);
		
	if ($b===false || strlen(trim($b))==0)
		$m['body']="";
	else
		$m['body']=trim($b);

	return FileWrite(MAILSESSION, serialize($m));
}

function ComposeCollectMail($nextCmd)
{
	global $TMP, $signature;

	$incSig=VarPost('includeSignature', false);
	if ($incSig!==false && strlen($signature)>0) {
		FileWrite($TMP."/includeSignature", "yes");
		$incSig=true;
	} else {
		@unlink($TMP."/includeSignature");
		$incSig=false;
	}

	$subject=VarPost('subject', "");
	$body=VarPost('body', "");
	if (strlen($subject)==0 && strlen($body)==0) {
		echo "<p><br /><b>Empty subject and body</b></p><br />";
		Compose($nextCmd, $subject, $body);
		return;
	}

	if ($incSig===false)
		$allBody=$body;
	else
		$allBody=$body."\n".$signature;

	if (StoreSession($subject, $allBody)===false) {
		echo "<p><br /><b>Failed to store data on server!</b></p><br />";
		Compose($nextCmd, $subject, $body);
	} else
		CollectAddresses("S");
}

function GetAddrBlock($hdr, $dontInclude)
{
	$addr="";
	$sep="";
	while (list($k, $v)=each($hdr)) {
		$a=$v->mailbox."@".$v->host;
		if ($dontInclude!==false) {
			if (strcasecmp($dontInclude, $a)==0)
				continue;
		}
		$addr.=$sep.$a;
		$sep=",";
	}

	return $addr;
}

function CollectAddresses($nextCmd)
{
	global $page, $mail, $segment, $mBox, $hdrFrom;

	$hdr="<a href=\"footmail.php?p=".$page."\" accesskey=\"1\">Inbox</a>&nbsp;&nbsp;<a href=\"index.html\" accesskey=\"2\">Menu</a>";

	echo "<p>".$hdr."</p><p>Enter addresses, separated by comma (','):</p><form method=\"post\" action=\"footmail.php\"><input type=\"hidden\" name=\"c\" value=\"".$nextCmd."\" /><input type=\"hidden\" name=\"p\" value=\"".$page."\" />";

	$to="";
	$cc="";
	if ($mail!==false) {
		echo "<input type=\"hidden\" name=\"m\" value=\"".$mail."\" />";
		if (($msg=@imap_fetchheader($mBox, $mail))!==false) {
			$msg=@imap_rfc822_parse_headers($msg);

			if (isset($msg->from)==true && $msg->from!==false)
				$from=GetAddrBlock($msg->from, "");

			if (isset($msg->to)==true && $msg->to!==false)
				$to=GetAddrBlock($msg->to, $hdrFrom);

			if (strlen($to)>0)
				$to=$from.",".$to;
			else
				$to=$from;

			if (isset($msg->cc)==true && $msg->cc!==false)
				$cc=GetAddrBlock($msg->cc, $hdrFrom);
		}
	}

	echo "<p><b>To:</b></p>";
	echo "<input type=\"text\" name=\"to\" size=\"20\" maxlength=\"1024\" value=\"".$to."\" />";
	echo "<p><b>Cc:</b></p>";
	echo "<input type=\"text\" name=\"cc\" size=\"20\" maxlength=\"1024\" value=\"".$cc."\" />";
	echo "<p><b>Bcc:</b></p>";
	echo "<input type=\"text\" name=\"bcc\" size=\"20\" maxlength=\"1024\" />";
	echo "<p><input type=\"submit\" value=\"Send\" accesskey=\"6\"/></p>";
	echo "</form>";

	echo "<p>".$hdr."</p>";
}

?>
