<?php
	require_once('../../../wp-config.php');

	$fc = get_option('filecache');
	$doc = $fc[path] . "/" . basename($_GET['f']);

	if ((is_user_logged_in()) &&
	    (file_exists($doc))) {
		unlink($doc);
	}

	header("Location: http://" . $_SERVER['HTTP_HOST'] . "/wordpress/wp-admin/upload.php?page=filecache.php");
?>
