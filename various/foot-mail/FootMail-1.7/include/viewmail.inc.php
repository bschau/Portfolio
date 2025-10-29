<?php
	require_once('messagecomponents.inc.php');

function ViewMail()
{
	global $contentBreak, $content, $knownAttachments, $unknownAttachments;
	global $mail, $timeZone;
	global $mBox, $page;

	$hdr="<a href=\"footmail.php?p=".$page."\">Inbox</a>&nbsp;&nbsp;<a href=\"index.html\">Menu</a>&nbsp;&nbsp;<a href=\"footmail.php?c=R&m=".$mail."\">Reply</a>&nbsp;&nbsp;<a href=\"footmail.php?c=F&m=".$mail."\">Forward</a>&nbsp;&nbsp;";

	echo "<p>".$hdr."</p>";

	if (($res=MBMessageInfo($mBox, $mail))==false) {
		echo "<p>Invalid message</p>";
		return;
	}

	$nD=strftime("%Y%m%d", time()+(3600*$timeZone));
	$d=substr($res['date'], 0, 8);
	$t=substr($res['date'], 8);
	echo "<p><b>Date:</b> ";
	if ($nD!=$d)
		echo substr($d, 0, 4)."/".substr($d, 4, 2)."/".substr($d, 6)."&nbsp;";
	echo substr($t, 0, 2).":".substr($t, 2);
	echo "<br /><b>From:</b> ";
	if (strlen($res['personal'])>0)
		echo $res['personal'];
	else
		echo $res['from'];

	echo "<br /><b>Subject:</b> ";
	$e=@imap_mime_header_decode($res['subject']);
	if (($cnt=count($e))>0) {
		for ($i=0; $i<$cnt; $i++)
			echo htmlentities($e[$i]->text);
	} else
		echo "N/A";
	
	echo "</p>";

	$msg=new message_components($mBox);
	$msg->fetch_structure($mail);

	$content=false;
	$contentBreak="";
	$knownAttachments="";
	$unknownAttachments="";
	while (list($k, $v)=each($msg->file_type[$mail])) {
		if ($v=='text/plain') {
			if (($c=MBGetBody($mBox, $mail, $msg->pid[$mail][$k], $msg->encoding[$mail][$k]))===false)
				$content.=$contentBreak."?";
			else
				$content.=$contentBreak.htmlentities($c);

			$contentBreak="\n";
		} else if ($v=='text/html') {
			if (isset($msg->fname[$mail][$k])) {
				if (strlen($msg->fname[$mail][$k])>0)
					$n=$msg->fname[$mail][$k];
				else
					$n="Unnamed HTML file";
				
				$knownAttachments.="<a href=\"footmail.php?p=".$page."&c=A&m=".$mail."&s=".$msg->pid[$mail][$k]."\">".$n."</a><br />";
			}
		} else {
			if (isset($msg->fname[$mail][$k])) {
				if (strlen($msg->fname[$mail][$k])>0)
					$unknownAttachments=$msg->fname[$mail][$k]."<br />";
				else
					$unknownAttachments="Unnamed file<br />";
			}
		}
	}

	if ($content===false)
		echo "<p>There is no viewable body in this message</p>";
	else
		echo "<p>".nl2br($content)."</p>";

	if (strlen($knownAttachments)>0)
		echo "<p><b>Viewable attachments:</b><br />".$knownAttachments."</p>";

	if (strlen($unknownAttachments)>0)
		echo "<p><b>Unknown attachments:</b><br />".$unknownAttachments."</p>";

	echo "<p>View: <a href=\"footmail.php?p=".$page."&c=B&m=".$mail."\">Body</a>&nbsp;&nbsp;<a href=\"footmail.php?p=".$page."&c=H&m=".$mail."\">Headers</a></p>";
	echo "<p>".$hdr."</p>";
}

?>
