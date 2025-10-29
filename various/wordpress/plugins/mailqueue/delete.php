<?php
	require_once('../../../wp-config.php');

	if (is_user_logged_in()) {
		if (isset($_GET['m'])) {
			require_once('wMailQueue.inc.php');

			$id = $_GET['m'];
			$mq = new wMailQueue();
			$mailQueue = $mq->getQueue();

			if (file_exists($mailQueue . "/c" . $id)) {
				unlink($mailQueue . "/c" . $id);
			}

			if (file_exists($mailQueue . "/m" . $id)) {
				unlink($mailQueue . "/m" . $id);
			}
	
			if (file_exists($mailQueue . "/t" . $id)) {
				unlink($mailQueue . "/t" . $id);
			}
		}
	}

	header("Location: http://" . $_SERVER['HTTP_HOST'] . "/wordpress/wp-admin/tools.php?page=mailqueue.php");
?>
