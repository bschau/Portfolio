<?php
	require_once('messagecomponents.inc.php');

function HtmlRemove($src)
{
	if ($src===false)
		return "";

	$search=array("'<script[^>]*?>.*?</script>'si",
			"'<[/!]*?[^<>]*?>'si",
			"'([rn])[s]+'",
			"'&(quot|#34);'i",
			"'&(amp|#38);'i",
			"'&(lt|#60);'i",
			"'&(gt|#62);'i",
			"'&(nbsp|#160);'i",
			"'&(iexcl|#161);'i",
			"'&(cent|#162);'i",
			"'&(pound|#163);'i",
			"'&(copy|#169);'i",
			"'&#(d+);'e");

	$replace=array("", "", "\1", "\"", "&", "<", ">", " ", chr(161), chr(162), chr(163), chr(169), "chr(\1)");

	return preg_replace($search, $replace, $src);
}

function ViewAttachment()
{
	global $mBox, $mail, $segment;

	$msg=new message_components($mBox);
	$msg->fetch_structure($mail);

	if (($content=MBGetBody($mBox, $mail, $segment, $msg->encoding[$mail][$segment]))===false) {
		echo "<p>Unknown attachment encoding</p>";
		return;
	}

	switch ($msg->file_type[$mail][$segment]) {
	case 'text/plain':
		echo "<pre>".htmlentities($content)."</pre>";
		break;
	case 'text/html':
		echo "<pre>".HtmlRemove($content)."</pre>";
		break;
	default:
		echo "<p>Cannot handle ".$msg->file_type[$mail][$segment]."</p>";
		break;
	}
}

?>


