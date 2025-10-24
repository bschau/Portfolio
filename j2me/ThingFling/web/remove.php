<?php
	require_once('fb.inc.php');

	$uid=$fb->get_loggedin_user();
	if (($uid!=NULL) && ($fb->fb_params['uninstall']==1)) {
		$db=sqlite_open("/usr/home/schau/WebDyn/thingfling.db") or die("-Failed to open database");
		$u=sqlite_escape_string($uid);
		$sql="DELETE FROM users WHERE uid='".$u."'";
		sqlite_query($db, $sql);
	}
?>
