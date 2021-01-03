<?php
	$preamble = "Called: ". time();
	$msg = "";

	if ($_SERVER['REQUEST_METHOD'] === 'POST') {
		$msg = "POST: " . file_get_contents('php://input');
	} else if ($_SERVER['REQUEST_METHOD'] === 'GET') {
		$msg = "GET: " . $_SERVER['QUERY_STRING'];
	} else {
		$msg = "Method: " . $_SERVER['REQUEST_METHOD'];
	}

	syslog(LOG_INFO, $preamble . "\n" . $msg);
?>
