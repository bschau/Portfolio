<?php
	if (php_uname('n')=='stacy') {
		$db="/home/bsc/Projects/Webs/WebDyn/thingfling.db";
	} else {
		$db="/usr/home/schau/WebDyn/thingfling.db";
	}

	if (file_exists($db)) {
		echo "Database already exists\n";
		exit;
	}

	$res=sqlite_open($db) or die("Cannot open/create database");
	sqlite_query($res, 'CREATE TABLE users (uid VARCHAR(16) PRIMARY KEY, pin VARCHAR(16), timestamp VARCHAR(16))');
	sqlite_close($res);
?>
