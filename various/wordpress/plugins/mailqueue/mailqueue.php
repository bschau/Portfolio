<?php
/*
 * Plugin Name: MailQueue
 * Plugin URI: http://www.smil-it.dk/
 * Description: A massmailer controller class. Put a .htaccess into the
 * mailqueue folder with the following content:
 *
 *     deny from all
 *
 * Version: 1.0
 * Author: Brian Schau
 * Author URI: http://www.smil-it.dk/
 */
	require_once('wMailQueue.inc.php');

	if (is_admin()) {
		add_action('admin_menu', 'MailQueueMenu');
		add_action('admin_init', 'MailQueueInit');
	}

function MailQueueMenu() {
	global $admin_page_hooks, $_registered_pages;

	add_management_page('Mail Queue', 'Mail Queue', 'administrator', 'mailqueue.php', 'MailQueue_main');
	add_options_page('Mail Queue', 'Mail Queue', 'administrator', 'mailqueue.php', 'MailQueue_options');
}

function MailQueueInit() {
	register_setting('mailqueue-options-group', 'mailqueue');

	wp_enqueue_style("MQStyles", WP_PLUGIN_URL . "/mailqueue/styles.css");
}

function mqGetFile($fn) {
	global $mailQueue;

	$t = $mailQueue ."/" . $fn;
	if (($fh = fopen($t, "rb")) !== false) {
		if (($d = fread($fh, filesize($t))) !== false) {
			fclose($fh);
			return stripslashes($d);
		}

		fclose($fh);
	}

	return false;
}

function mqUnserializeFile($fn) {
	if (($d = mqGetFile($fn)) === false) {
		return false;
	}

	return unserialize($d);
}

function MailQueue_main() {
	global $mailQueue;

	$mq = get_option('mailqueue');
	$mailQueue = $mq[queue];

	$self = $_SERVER['PHP_SELF'];
	$url = WP_PLUGIN_URL . "/mailqueue";

	echo <<<EOF
<div class="wrap">
<h2>Mail Queue</h2>
<p>Mail Queue Path: $mailQueue</p>
<script type="text/javascript">
var mqTimeout;

function mqPopup(parent, id) {
	clearTimeout(mqTimeout);

	var el = document.getElementById('mqPopup');

	el.innerHTML = '<a href="$url/delete.php?m=' + id + '">Slet</a>';
	var l = document.getElementById('mails');
	var spot = document.getElementById(parent);

	el.style.visibility = "visible";
	el.style.left = spot.offsetLeft + l.offsetLeft + "px";
	el.style.top = spot.offsetTop + l.offsetTop + "px";

	mqTimeout = setTimeout("document.getElementById('mqPopup').style.visibility = 'hidden'", 3000);
}
</script>
<div id="mqPopup"> </div>
<table class="widefat" id="mails">
<thead>
	<tr><th>ID</th><th>Control File</th><th>Message</th><th>Receipients</th></tr>
</thead>
<tbody>
EOF;
	$files = scandir($mailQueue);

	reset($files);
	$ids = array();
	while (list($k, $fn) = each($files)) {
		if ($fn[0] == "c") {
			$id = substr($fn, 1);
			$ids[$id] .= $id;
		}
	}

	ksort($ids);
	reset($ids);
	$elem = 1;
	while (list($k, $id) = each($ids)) {
		echo "<tr id=\"pop" . $elem . "\"><td><a href=\"javascript:mqPopup('pop" . $elem . "', '" . $id . "')\">" . $id . "</td><td>";
		       
		$ctl = mqUnserializeFile("c" . $id);
		if ($ctl === false) {
			echo "Read error";
		} else {
			ksort($ctl);
			reset($ctl);
			while (list($k, $v) = each($ctl)) {
				echo nl2br($v);
			}
		}

		echo "</td><td>";
		echo mqGetFile("m" . $id);
		echo "</td><td>";

		$rcpt = mqUnserializeFile("t" . $id);
		if ($rcpt === false) {
			echo "Read error";
		} else {
			ksort($rcpt);
			reset($ctl);
			while (list($k, $v) = each($rcpt)) {
				echo " " . $v;
			}
		}
		echo "</td></tr>";

		$elem++;
	}

	echo <<<EOF
</tbody>
</table>
</div>
EOF;
}

function MailQueue_options() {
	echo <<<EOF
<div class="wrap">
<h2>Mail Queue Options</h2>
<form method="post" action="options.php">
EOF;

	settings_fields('mailqueue-options-group');

	$mq = get_option('mailqueue');

	echo <<<EOF
<table class="form-table">
<tr valign="top">
<th scope="row">Mail queue path</th>
<td><input type="text" name="mailqueue[queue]" value="$mq[queue]" /></td>
</tr>
<tr valign="top">
<th scope="row">Debug</th>
<td><input type="text" name="mailqueue[debug]" value="$mq[debug]" /></td>
</tr>
<tr valign="top">
<th scope="row">Halted</th>
<td><input type="text" name="mailqueue[halted]" value="$mq[halted]" /></td>
</tr>
</table>
<p class="submit">
<input type="submit" class="button-primary" value="
EOF;
	_e("Save Changes");
	echo <<<EOF
" /></p>
</form>
</div>
EOF;
}

?>
