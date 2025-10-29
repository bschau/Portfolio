<?php
	require_once('../../../wp-config.php');

	$fc = get_option('filecache');
	$doc = $fc[fcpath] . "/" . basename($_GET['f']);

	if ((is_user_logged_in()) &&
		(file_exists($doc))) {
		$fn = base64_decode(basename($doc));
		@ob_end_clean();
		@ini_set('zlib.output_compression', 'Off');
		header('Pragma: public');
		header('Last-Modified: ' . gmdate('D, d M Y H:i:s') . ' GMT');
		header('Cache-Control: no-store, no-cache, must-revalidate');
		header('Cache-Control: post-check=0, pre-check=0, max-age=0');
		header('Content-Transfer-Encoding: none');
		if (isset($_SERVER['HTTP_USER_AGENT']) && preg_match("/MSIE/", $_SERVER['HTTP_USER_AGENT'])) {
			header('Content-Type: application/octetstream; name="' . $fn . '"');
		} else {
			header('Content-Type: application/octet-stream; name="' . $fn . '"');
		}
		header('Content-Disposition: inline; filename="' . $fn . '"');
		header('Content-Length: ' . filesize($doc));
		readfile($doc);
	}

?>
