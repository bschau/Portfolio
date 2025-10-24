<?php

function login($uid, $pin) {
	global $db;

	$db=sqlite_open("/usr/home/schau/WebDyn/thingfling.db") or die("-Failed to open database");
	$u=sqlite_escape_string($uid);
	$sql="SELECT pin FROM users WHERE uid='".$u."'";
	$res=sqlite_query($db, $sql);
	if (sqlite_num_rows($res)==1) {
		$arr=sqlite_fetch_array($res, SQLITE_ASSOC);
		if ($arr['pin']==$pin) {
			$sql="UPDATE users SET timestamp='".time()."' WHERE uid='".$u."'";
			sqlite_query($db, $sql);
			sqlite_close($db);
			return;
		}
	}

	sqlite_close($db);
	die("-Login error");
}

	if (isset($_POST['uid'])) {
		if (isset($_POST['pin'])) {
			$uid=$_POST['uid'];
			login($uid, $_POST['pin']);
		} else {
			die("-Login error");
		}
	} else {
		die("-Login error");
	}

	if (isset($_POST['status'])) {
		$setStatus=$_POST['status'];
	} else {
		$setStatus=false;
	}

	require_once('fb.inc.php');

	if ($setStatus) {
		$status=trim(urldecode($_POST['status']));
		if (strlen($status)==0) {
			$status="???";
		}

		$fb->api_client->users_setStatus(utf8_encode($status), $uid);
		echo "+";
	} else {
		die("-Invalid command");
	}
?>
