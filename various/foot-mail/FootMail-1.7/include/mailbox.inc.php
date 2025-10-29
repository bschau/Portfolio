<?php

function MBOpen()
{
	global $mbServer, $mbPort, $mbServerUser, $mbServerPass;

	for ($cnt=0; $cnt<5; $cnt++) {
		if (($m=@imap_open("{".$mbServer.":".$mbPort."/pop3}INBOX", $mbServerUser, $mbServerPass))!==false)
			return $m;

		sleep(1);
	}

	return false;
}

function MBClose()
{
	global $mBox;

	if ($mBox!=false) {
		@imap_close($mBox);
		$mBox=false;
	}
}

/*
 * This function was more or less taken from mimeDecode from PEAR::Mail_Mime
 */
function MBDecode($src)
{
	$src=preg_replace('/(=\?[^?]+\?(q|b)\?[^?]*\?=)(\s)+=\?/i', '\1=?', $src);
	while (preg_match('/(=\?([^?]+)\?(q|b)\?([^?]*)\?=)/i', $src, $matches)) {
		$encoded=$matches[1];
		$charset=$matches[2];
		$encoding=$matches[3];
		$text=$matches[4];

		switch (strtolower($encoding)) {
		case 'b':
			$text=base64_decode($text);
			break;

		case 'q':
			$text=str_replace('_', ' ', $text);
			preg_match_all('/=([a-f0-9]{2})/i', $text, $matches);
			foreach ($matches[1] as $value)
				$text=str_replace('='.$value, chr(hexdec($value)), $text);
			break;
		}

		$src=str_replace($encoded, $text, $src);
	}

	return $src;
}

function MBList($mBox)
{
	$res=array();
	$resCnt=0;

	$num=@imap_num_msg($mBox);
	$o=@imap_fetch_overview($mBox, "1:".$num, 0);
	if (is_array($o)) {
		reset($o);
		while (list($k, $v)=each($o)) {
			$addr=@imap_rfc822_parse_adrlist($v->from, "");
			reset($addr);
			$from="<>";
			$personal="";
			if (is_array($addr) && count($addr)>0) {
				$a=$addr[0];
				if (isset($a->mailbox) && isset($a->host))
					$from=$a->mailbox."@".$a->host;

				if (isset($a->personal))
					$personal=MBDecode($a->personal);
			}

			$res[$resCnt]['uid']=$v->uid;
			$res[$resCnt]['size']=$v->size;
			$res[$resCnt]['date']=strftime("%Y%m%d%H%M", strtotime($v->date));
			$res[$resCnt]['from']=$from;
			$res[$resCnt]['personal']=$personal;
			if (isset($v->subject) && strlen(trim($v->subject))>0)
				$res[$resCnt]['subject']=MBDecode($v->subject);
			else
				$res[$resCnt]['subject']='N/A';

			$resCnt++;
		}

		$sortAux=array();
		foreach ($res as $r)
			$sortAux[]=$r['date'];

		array_multisort($sortAux, SORT_DESC, $res);

		return $res;
	}

	return false;
}

function MBFilter($res, $filter)
{
	reset($res);

	$max=count($res);
	for ($idx=0; $idx<$max; $idx++) {
		if (stristr($res[$idx]['subject'], $filter)!==false)
			continue;

		if (stristr($res[$idx]['from'], $filter)!==false)
			continue;

		if (stristr($res[$idx]['personal'], $filter)!==false)
			continue;

		unset($res[$idx]);
	}

	return array_values($res);
}

function MBPage($res, $start, $span)
{
	reset($res);
	$result=array();
	$max=count($res);
	$cnt=0;
	for (; $start<$max; ) {
		if ($span>0) {
			$result[$cnt++]=$res[$start];
			$span--;
			$start++;
		} else
			break;
	}

	return $result;
}

function MBMessageInfo($mBox, $msgNum)
{
	$res=array();

	if ($msgNum>@imap_num_msg($mBox))
		return false;

	$o=@imap_fetch_overview($mBox, $msgNum, 0);
	if (is_array($o)) {
		reset($o);
		while (list($k, $v)=each($o)) {
			$addr=@imap_rfc822_parse_adrlist($v->from, "");
			reset($addr);
			$from="<>";
			$personal="";
			if (is_array($addr) && count($addr)>0) {
				$a=$addr[0];
				if (isset($a->mailbox) && isset($a->host))
					$from=$a->mailbox."@".$a->host;

				if (isset($a->personal))
					$personal=MBDecode($a->personal);
			}

			$res['uid']=$v->uid;
			$res['size']=$v->size;
			$res['date']=strftime("%Y%m%d%H%M", strtotime($v->date));
			$res['from']=$from;
			$res['personal']=$personal;
			if (isset($v->subject) && strlen(trim($v->subject))>0)
				$res['subject']=MBDecode($v->subject);
			else
				$res['subject']='N/A';
		}
	}

	return $res;
}

function MBGetBody($mBox, $mail, $part, $encoding)
{
	$content=@imap_fetchbody($mBox, $mail, $part);

	if ($encoding=='7bit' or $encoding=='8bit')
		return $content;

	if ($encoding=='base64')
		return @base64_decode($content);

	if ($encoding=='quoted-printable')
		return @quoted_printable_decode($content);

	return false;
}

?>
