<?php
	require_once('../../../wp-config.php');
	require_once('../../../wp-includes/wp-db.php');

	$mq = get_option('mailqueue');
	if ($mq[halted] == '1') {
		exit;
	}

	if ($mq[debug] == '1') {
		define("DEBUG", "define_to_activate");
	}

	define("QUEUE", $mq[queue]);

function debug($msg) {
	if (defined("DEBUG")) {
		echo $msg . "\n";
	}
}

function isMailBundle($fn) {
	if ($fn[0] == 'c') {
		$t = substr($fn, 1);
		if ((is_file(QUEUE . "/m" . $t)) && (is_file(QUEUE . "/t" . $t))) {
			debug("MailBundle for " . $fn . " intact");
			return true;
		}
	}

	debug("r or a file missing for " . $fn);
	return false;
}

function unserializeFile($t, $close = true) {
	$d = false;
	if (($fh = fopen($t, "rb")) !== false) {
		if (($d = fread($fh, filesize($t))) === false) {
			debug("Cannot read " . $t);
		}
	} else {
		debug("Cannot open " . $t);
	}

	if ($close) {
		fclose($fh);
	}

	if ($d !== false) {
		return unserialize($d);
	}

	return false;
}

function writeFile($name, $content) {
	if (($fh = fopen($name, "wb+")) !== false) {
		if (fwrite($fh, serialize($content)) !== false) {
			fclose($fh);
			return true;
		} else {
			debug("Cannot write " . $name);
		}

		fclose($fh);
	} else {
		debug("Cannot open " . $name . " for writing");
	}

	return false;
}

function encodeUTF8($str) {
	debug("encodeUTF8 input: " . $str);

	$txt = '=?utf-8?q?';

	$strLen = strlen($str);
	for ($i = 0; $i < $strLen; $i++) {
		$v = ord($str[$i]);

		if (($v > 127) or ($v == 63)) {
			$v = dechex($v);
			$txt .= '=';
			$txt .= $v;
		} else {
			$txt .= $str[$i];
		}
	}

	$txt .= '?=';

	debug("encodeUTF8 out: " . $txt);
	return $txt;
}

function sendMail($fn) {
	$t = substr($fn, 1);

	$addressFile = QUEUE . "/t" . $t;
	$msgFile = QUEUE . "/m" . $t;
	$ctlFile = QUEUE . "/c" . $t;

	debug("sendMail: " . $ctlFile . "/" . $msgFile . "/" . $addressFile);
	if (($to = unserializeFile($addressFile, true)) === false) {
		debug("Cannot read toFile (" . $addressFile . ")");
		return;
	}

	if (($fh = fopen($msgFile, "rb")) === false) {
		debug("Cannot open msgFile " . $msgFile);
		return;
	}

	$msg = fread($fh, filesize($msgFile));
	fclose($fh);

	if ($msg === false) {
		debug("Failed to read msgFile " . $msgFile);
		return;
	}

	if (($ctl = unserializeFile($ctlFile)) === false) {
		debug("Cannot read ctlFile " . $ctlFile);
		return;
	}

	reset($to);
	while (list($k, $email) = each($to)) {
		$subject = encodeUTF8($ctl["subject"]);
		debug("Sending to: " . $email . "  with subject: " . $subject);
		if (mail($email, $subject, $msg, $ctl["header"], $ctl["params"]) === true) {
			unset($to[$k]);
			if ((writeFile($addressFile, $to)) === false) {
				debug("Cannot rewrite addressFile " . $addressFile);
				return;
			}
		} else {
			debug("Cannot send mail to " . $email . " from " . $ctlFile);
			writeFile($addressFile, $to);
			return;
		}
	}

	unlink($addressFile);
	unlink($msgFile);
	unlink($ctlFile);
}

	if (isset($_SERVER['DOCUMENT_ROOT'])) {
		$dr = trim($_SERVER['DOCUMENT_ROOT']);
		if (strlen($dr) > 0) {
			die("Cannot run in a web server context ...");
		}
	}

	$files = scandir(QUEUE);

	reset($files);
	while (list($k, $fn) = each($files)) {
		if ($fn[0] == ".") {
			continue;
		}

		$f = QUEUE . "/" . $fn;

		if (is_file($f)) {
			if (isMailBundle($fn)) {
				if (($fp = fopen($f, "rb")) === false) {
					continue;
				}

				if (flock($fp, LOCK_EX | LOCK_NB) === true) {
					sendMail($fn);
				} else {
					debug("Failed to lock file " . $f);
				}

				fclose($fp);
			}
		}
	}
?>
