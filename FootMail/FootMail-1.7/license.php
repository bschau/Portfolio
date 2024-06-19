<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//WAPFORUM//DTD XHTML Mobile 1.0//EN" "http://www.wapforum.org/DTD/xhtml-mobile10.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<title>GNU General Public License v2</title>
	</head>

	<body>
	<p><a href="index.html">Back</a></p>
<?php
	if (is_file('docs/LICENSE.txt')===true) {
		$c=file_get_contents('docs/LICENSE.txt');

		$c=strtr($c, "\x0c", " ");
		$c=str_replace("\0x09", "        ", $c);
		echo "<pre>".htmlentities($c)."</pre>";
	} else
		echo "<p>License file not found.</p>";
?>
	<p><a href="index.html">Back</a></p>
	</body>
</html>
