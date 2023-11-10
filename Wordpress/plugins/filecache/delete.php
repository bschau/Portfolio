<?php
	require_once('../../../wp-config.php');

	$fc = get_option('filecache');
	$doc = $fc[fcpath] . "/" . basename($_GET['f']);

	if ((is_user_logged_in()) &&
	    (file_exists($doc))) {
		unlink($doc);
	}

	$url = empty($_SERVER['HTTPS']) ? "http://" : "https://";
	$url .= $_SERVER['HTTP_HOST'] . "/wp-admin/upload.php?page=filecache.php";
	header("Location: " . $url);
?>
