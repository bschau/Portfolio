<?php

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

function UpdateCache()
{
	global $errStr, $mbServer, $mbPort, $mbServerUser, $mbServerPass, $TMP;

	if (($sck=@fsockopen($mbServer, $mbPort, $errno, $errstr, 30))===false) {
		$errStr="Cannot open connection to mailbox (SOCKET)";
		return false;
	}

	if (($ret=@fgets($sck, 8192))===false) {
		$errStr="Cannot connect";
		return false;
	}

	if (strncasecmp("+OK", $ret, 3)!=0) {
		$errStr="Server is not ready";
		return false;
	}

	if (PingPong($sck, "USER ".$mbServerUser)===false) {
		$errStr="Cannot send USER";
		return false;
	}

	if (PingPong($sck, "PASS ".$mbServerPass)===false) {
		$errStr="Cannot send PASS";
		return false;
	}

	if (($ret=PingPong($sck, "LIST"))===false) {
		$errStr="Cannot send LIST";
		return false;
	}

	$content="";
	while (!feof($sck)) {
		if (($ret=@fgets($sck, 8192))===false) {
			$errStr="Cannot get from stream";
			fclose($sck);
			return false;
		}

		if ($ret{0}==='.')
			break;

		$content.="-".trim($ret);
	}
	
	fclose($sck);

	if (is_file($TMP."/DynCache.inc.php")===true) {
		if (($DynCache=FileUnserialize($TMP."/DynCache.inc.php"))===false) {
			$errStr="Cannot read DynCache";
			return false;
		}
	} else
		$DynCache="";

	if ($content==$DynCache) {
		if (is_file($TMP."/DynList.inc.php")==true) {
			if (($DynList=FileUnserialize($TMP."/DynList.inc.php"))===false) {
				$errStr="Cannot read DynList";
				return false;
			}

			return $DynList;
		}

		return array();		// No messages
	}

	
	if (($mBox=MBOpen())===false) {
		$errStr="Cannot open connection to mailbox (IMAP)";
		return false;
	}

	$res=MBList($mBox, false);
	imap_close($mBox);

	if ($res===false) {
		$errStr="Failed to list inbox";
		return false;
	}

	if (FileWrite($TMP."/DynList.inc.php", serialize($res))===true) {
		if (FileWrite($TMP."/DynCache.inc.php", serialize($content))===true)
			return $res;
		else
			$errStr="Cannot update DynCache";
	} else
		$errStr="Cannot update DynList";

	return false;
}

?>
