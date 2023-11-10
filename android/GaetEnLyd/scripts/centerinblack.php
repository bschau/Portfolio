<?php
	$w = 200;
	$h = 300;

	$dst = imagecreatetruecolor($w, $h);
	$black = imagecolorallocate($dst, 0, 0, 0);
	imagefill($dst, 0, 0, $black);

	$src = imagecreatefromjpeg($argv[1]);
	$x = imagesx($src);
	$y = imagesy($src);

	imagecopy($dst, $src, ($w - $x) / 2, ($h - $y) / 2, 0, 0, $x, $y);
	imagepng($dst, "../p/" . basename($argv[1], ".jpg") . ".png");
?>
