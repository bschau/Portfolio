<style type="text/css">
th.ft { text-align: left; }
td.ft { text-align: left; }
.ftmain {
	background-color: #f7f7f7;
	border: 1px solid #bdc7d8;
	padding: 5px;
	font-family: "lucida grande",tahoma,verdana,arial,sans-serif;
	font-size: 11px;
	width: 330px;
}

.fthead {
	margin-bottom: 10px;
	padding: 5px 0 0 5px;
	font-size: 13px;
}

input {
	border: 1px solid #bdc7d8;
	font-family: "lucida grande",tahoma,verdana,arial,sans-serif;
	font-size: 11px;
	padding: 3px;
}

input:focus {
	border-color: #687fb0;
}

.error {
	background-color: #ffeedd;
	color: #ff3322;
	margin: 10px;
	font-size: 13px;
	font-family: "lucida grande",tahoma,verdana,arial,sans-serif;
	text-align: center;
	border: 1px dotted #ff3322;
}

.message {
	background-color: #ddeeff;
	color: #2233ff;
	margin: 10px;
	font-size: 13px;
	font-family: "lucida grande",tahoma,verdana,arial,sans-serif;
	text-align: center;
	border: 1px dotted #2233ff;
}
</style>
<?php
	require_once('fb.inc.php');

function hasExtPerm($fb, $uid, $perm) {
	$result=$fb->api_client->call_method('facebook.users.hasAppPermission', array(
		'session_key' => $fb->api_client->session_key,
		'ext_perm' => $perm,
		'uid' => $uid));

	return (intval((string)$result)==1) ? true : false;
}

function GetPost($var, $errTxt) {
	global $err;

	if (isset($_POST[$var])) {
		$v=trim($_POST[$var]);
		if (strlen($v)>0) {
			return $v;
		}
	}

	$err=$errTxt;
	return false;
}

function ValidatePwds() {
	global $err, $pwd;

	if (($pwd=GetPost('newPin', '<b>New Pin</b> is not valid'))===false) {
		return false;
	}

	if (($rep=GetPost('repeat', '<b>Repeat</b> is not valid'))===false) {
		return false;
	}

	if ($pwd==$rep) {
		return true;
	}

	$err="PIN Code mismatch";
	return false;
}

function OldPwdMatch() {
	global $pin, $pwd, $err;

	if ($pin===false) {
		return true;
	}

	if (($oldPin=GetPost('oldPin', '<b>Old Pin</b> is not valid'))===false) {
		return false;
	}

	if ($pin==$oldPin) {
		return true;
	}

	$err="Incorrect old Pin";
	return false;
}

function dbOpen() {
	global $db;

	if ($db==false) {
		$db=sqlite_open("/usr/home/schau/WebDyn/thingfling.db");
	}
}

function dbUpdate() {
	global $db, $uid, $pin;

	dbOpen();

	$u=sqlite_escape_string($uid);
	$sql="DELETE FROM users WHERE uid='".$u."'";
	sqlite_query($db, $sql);

	$sql="INSERT INTO users(uid, pin, timestamp) VALUES('".$u."', '".sqlite_escape_string($pin)."', '".time()."')";
	sqlite_query($db, $sql);
}

function dbRead() {
	global $db, $uid, $pin;

	dbOpen();

	$sql="SELECT pin,timestamp FROM users WHERE uid='".sqlite_escape_string($u)."'";
	$res=sqlite_query($db, $sql);
	if (sqlite_num_rows($res)==1) {
		$arr=sqlite_fetch_array($res, SQLITE_ASSOC);
		$pin=$arr['pin'];
	} else {
		$pin=false;
	}
}

	$db=false;
	$user=$fb->require_login();
	$uid=$fb->api_client->users_getLoggedInUser();
	$url=$fb->get_facebook_url('apps')."/thingfling/";

	$apiKey=FBAPIKEY;
	$err=false;
	$msg=false;
	if (isset($_GET['form'])) {
		echo <<<EOF
<div class="ftmain">
	<h3 class="fthead">Thing Fling Preferences</h3>
EOF;

		if (hasExtPerm($fb, $uid, 'email')==false) {
			$u=urlencode($url."?form=reset");
			echo <<<EOF
		<p>You need to grant Thing Fling permission to send an email to you with your new Pin. Please allow Thing Fling <a href="http://www.facebook.com/authorize.php?api_key=$apiKey&v=1.0&ext_perm=email&next=$u">to send an email to you</a>.</p>
</div>
EOF;
			exit;
		}

		srand(time());
		$pin="".rand()%9;
		$pin.="".rand()%9;
		$pin.="".rand()%9;
		$pin.="".rand()%9;

		dbUpdate($uid, $pin);

		$res=$fb->api_client->notifications_sendEmail($uid, "Thing Fling Pin", "Your new Thing Fling Pin is: ".$pin, "<p>Your new Thing Fling Pin is:</p><p><b>".$pin."</b>");

		echo <<<EOF
<p>The email with your new Thing Fling Pin has been sent!</p>
<p>Please <a href="$url">change your Pin</a> by using the Pin in the email as your <b>Old Pin</b>.</p>
EOF;
		exit;
	}

	dbRead();
	if (strlen(trim($pin))==0) {
		$pin=false;
	}

	if (isset($_POST['SetPrf'])) {
		if (ValidatePwds()) {
			if (OldPwdMatch()) {
				$pin=$pwd;
				dbUpdate();
				$msg="New Pin set";
				$pin=true;
			}
		}
	}

	$appPerm=hasExtPerm($fb, $uid, 'status_update');
?>
<div class="ftmain">
	<h3 class="fthead">Thing Fling Preferences</h3>
<?php
	if ($err!==false) {
		echo "<p class=\"error\">".$err."</p>";
	} else if ($msg!==false) {
		echo "<p class=\"message\">".$msg."</p>";
	}

	if ($appPerm===false) {
		$u=urlencode($url);
		echo <<<EOF
<p>You need to grant Thing Fling permission to update your status. Please allow Thing Fling <a href="http://www.facebook.com/authorize.php?api_key=$apiKey&v=1.0&ext_perm=status_update&next=$u">to update your status</a>.</p>
EOF;
	} else {
		echo <<<EOF
<p>Your UID (User ID) to be set in the Thing Fling application on your mobile phone is: <b>$uid</b>.</p>
<p><form method="post" action="$url">
<input type="hidden" name="SetPrf" value="1" />
<table>
EOF;
		if ($pin!==false) {
			echo <<<EOF
<tr><th class="ft">Old PIN:</th><td><input type="password" name="oldPin" size="16" maxlength="16" /></td></tr>
EOF;
		}

		echo <<<EOF
<tr><th class="ft">New PIN:</th><td><input type="password" name="newPin" size="16" maxlength="16" /></td></tr>
<tr><th class="ft">Repeat:</th><td><input type="password" name="repeat" size="16" maxlength="16" /></td></tr>
<tr><th><input type="submit" value="submit" /></a></th><td><a href="$url?form=reset">Reset Pin</a></td></tr>
</table>
</form>
</p>
<p>To install the Thing Fling mobile client open the web browser in your mobile phone and go to <a href="http://www.schau.com/ota.php">http://www.schau.com/ota.php</a>. For more information see <a href="http://www.facebook.com/apps/application.php?id=49996225389">the Thing Fling About page</a>.</p>
EOF;
	}
?>	
</div>
