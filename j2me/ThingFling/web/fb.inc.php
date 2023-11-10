<?php
	define("FBAPIKEY", "21e49f5a4d900dd02000807b73487215");
	define("FBSECRET", "d2d3d8ba862a9f74950d3c37813bce5f");

	require_once('facebook.php');

	$fb=new Facebook(FBAPIKEY, FBSECRET);
?>
