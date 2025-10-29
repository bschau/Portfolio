<?php
	require_once('cache.inc.php');

function ListInbox($page, $filter)
{
	global $pageSize, $timeZone, $errStr, $chkDeleted;

	if (($res=UpdateCache())===false) {
		echo "<p>Cannot update cache</p>";
		return;
	}

	$hdr="<a href=\"footmail.php?p=0\" accesskey=\"1\">Inbox</a>&nbsp;&nbsp;<a href=\"index.html\" accesskey=\"2\">Menu</a>&nbsp;&nbsp;<a href=\"footmail.php?p=0&c=C\" accesskey=\"3\">New</a>&nbsp;&nbsp;<a href=\"search.html\" accesskey=\"0\">Search</a>";

	if ($filter!==false) {
		$baseQuery="?F=".urlencode($filter)."&";
		$res=MBFilter($res, $filter);
	} else {
		$filter="";
		$baseQuery="?";
	}

	$mailCnt=count($res);
	$result=MBPage($res, $page*$pageSize, $pageSize);

	$pages=intval($mailCnt/$pageSize);
	if (($mailCnt%$pageSize)!=0)
		$pages++;

	$pgHdr="";
	if ($pages>1) {
		if ($page==0)
			$pgHdr.="&lt;&lt;";
		else
			$pgHdr.="<a href=\"footmail.php".$baseQuery."c=I&p=".($page-1)." accesskey=\"4\">&lt;&lt;</a>";

		$pgHdr.="&nbsp;(".($page+1)."/".$pages.")&nbsp;";

		if ($page>=($pages-1))
			$pgHdr.="&gt;&gt;";
		else
			$pgHdr.="<a href=\"footmail.php".$baseQuery."c=I&p=".($page+1)."\" accesskey=\"6\">&gt;&gt;</a>";
	}

	echo "<p>".$hdr."<br />".$pgHdr."</p><form method=\"post\" action=\"footmail.php\"><input type=\"hidden\" name=\"p\" value=\"0\" /><input type=\"hidden\" name=\"F\" value=\"".$filter."\" />";

	$nD=strftime("%Y%m%d", time()+(3600*$timeZone));
	$max=count($result);
	if ($max>0) {
		for ($idx=0; $idx<$pageSize; $idx++) {
			if ($idx<$max) {
				echo "<p>";
				echo "<input type=\"checkbox\" name=\"deleteUid[]\" value=\"".$result[$idx]['uid']."\"";
				if ($chkDeleted===true)
					echo " checked";

				echo " />&nbsp;&nbsp;";
				$size=$result[$idx]['size'];
				if ($size>1048576) {
					$size/=1048576;
					$size=intval($size)."MB";
				} else if ($size>1024) {
					$size/=1024;
					$size=intval($size)."KB";
				}
				echo $size."&nbsp;&nbsp;";

				$d=substr($result[$idx]['date'], 0, 8);
				$t=substr($result[$idx]['date'], 8);
				if ($nD==$d)
					echo substr($t, 0, 2).":".substr($t, 2)."&nbsp;&nbsp;";
				else
					echo substr($d, 0, 4)."/".substr($d, 4, 2)."/".substr($d, 6)."&nbsp;&nbsp;";

				if (strlen($result[$idx]['personal'])>0)
					echo $result[$idx]['personal']."&nbsp;&nbsp;<br />";
				else
					echo $result[$idx]['from']."&nbsp;&nbsp;<br />";

				$e=@imap_mime_header_decode($result[$idx]['subject']);
				$cnt=count($e);
				$u="";
				for ($i=0; $i<$cnt; $i++)
					$u.=$e[$i]->text;

				echo "<a href=\"footmail.php".$baseQuery."c=V&p=".$page."&m=".$result[$idx]['uid']."\">".htmlentities($u)."</a>";
				echo "</p>";
			} else
				break;
		}
	} else
		echo "<p>There is no mails to show. If you have done a search then the search may not have yielded any results. Tap the <a href=\"footmail.php?p=0\" accesskey=\"1\">Inbox</a> link to refresh the Inbox.</p>";

	echo "<p><input name=\"submitType\" type=\"submit\" value=\"Delete\" /></p></form>";
	echo "<p>".$pgHdr."<br />".$hdr."</p>";
}

?>
