<?php
$_inBold = false;
$_inItalic = false;
$_qotd = "";
$_result = array();

function qotd_get($qotd_file)
{
	global $_result;

	$content = file($qotd_file);
	_parse($content);
	reset($_result);
	return $_result[rand(0, count($_result))];
}

function _parse($content)
{
	global $_qotd;

	reset($content);
	$length = count($content);
	foreach ($content as $line) {
		$line = trim($line);
		if (_is_marker($line)) {
			_add_qotd();
			continue;
		}

		$s = _parse_line($line);
		$_qotd .= $s;
		$_qotd .= '<br />';
	}

	_add_qotd();
}

function _is_marker($line)
{
	return "%" === $line;
}

function _add_qotd()
{
	global $_qotd, $_result;

	$html = trim($_qotd);
	if (strlen($html) < 1) {
		return;
	}

	$_result[] .= $_qotd;
	$_qotd = "";
}

function _parse_line($line)
{
	global $_inBold, $_inItalic;

	$length = strlen($line);
	$text = "";

	for ($index = 0; $index < $length; $index++) {
		switch ($line[$index]) {
			case '\\':
				$index++;
				$text .= $line[$index];
				break;

			case '*':
				$text .= $_inBold ? "</b>" : "<b>";
				$_inBold = !$_inBold;
				break;

			case '_':
				$text .= $_inItalic ? "</i>" : "<i>";
				$_inItalic = !$_inItalic;
				break;

			case '#':
				$text .= '&nbsp;';
				break;

			default:
				$text .= $line[$index];
				break;
		}
	}

	return $text;
}

$qotd = htmlentities(qotd_get('qotds.txt'));
$ts = date('YmdHHiiss');
$l_time = localtime(time(), true);
$time = mktime(0, 0, 0, intval($l_time["tm_mon"]) + 1, $l_time["tm_mday"], intval($l_time["tm_year"]) + 1900);
$pub_date = date('r', $time);
$rss = <<<EOT
<?xml version="1.0" encoding="utf-8"?>
<rss version="2.0" xmlns:atom="http://www.w3.org/2005/Atom">
<channel>
	<title>Quote of the Day</title>
	<link>https://www.schau.dk/qotd.php</link>
	<description>Quote of the Day!</description>
	<atom:link href="https://www.schau.dk/qotd.php" rel="self" type="application/rss+xml" />
	<ttl>240</ttl>
	<pubDate>$pub_date</pubDate>
	<item>
		<title>Quote of the Day</title>
		<link>https://www.schau.dk/qotd.php?ts=$ts</link>
		<description>$qotd</description>
		<guid>https://www.schau.dk/qotd.php?ts=$ts</guid>
	</item>
</channel>
</rss>
EOT;

header("Content-Type: application/rss+xml");
echo $rss;
?>
