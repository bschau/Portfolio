<?php
/*
 * Plugin Name: FileCache
 * Plugin URI: http://www.schau.dk/
 * Description: A small file cache with upload / download functionality.
 * Put a .htaccess into the filecache folder with the following content:
 *
 * 	deny from all
 *
 * Version: 1.0
 * Author: Brian Schau
 * Author URI: http://www.schau.dk/
 */
	define("fcpath", "fcpath");
	if (is_admin()) {
		add_action('admin_menu', 'FileCacheMenu');
		add_action('admin_init', 'FileCacheInit');
	}

function FileCacheMenu() {
	add_media_page('File Cache', 'File Cache', 'publish_posts', 'filecache.php', 'FileCache_main');
	add_options_page('File Cache', 'File Cache', 'administrator', 'filecache.php', 'FileCache_options');

	fcFakePage('Upload', 'fcUpload', 'FileCache_upload');
}

function fcFakePage($title, $file, $function) {
	global $admin_page_hooks, $_registered_pages;

	$file = plugin_basename($file);
	$admin_page_hooks[$file] = sanitize_title($title);
	$hookname = get_plugin_page_hookname($file, '');
	add_action($hookname, $function);
	$_registered_pages[$hookname] = true;
}

function FileCacheInit() {
	register_setting('filecache-options-group', 'filecache');

	wp_enqueue_style("FCStyles", WP_PLUGIN_URL . "/filecache/styles.css");
}

function FileCache_main() {
	$self = $_SERVER['PHP_SELF'];
	$url = WP_PLUGIN_URL . "/filecache";

	echo <<<EOF
<div class="wrap">
<h2>File Cache</h2>
<script type="text/javascript">
var fcTimeout;

function fcPopup(parent, id) {
	clearTimeout(fcTimeout);

	var el = document.getElementById('fcPopup');

	el.innerHTML = '<a href="$url/download.php?f=' + id + '">Download</a> | <a href="$url/delete.php?f=' + id + '">Delete</a>';
	var l = document.getElementById('files');
	var spot = document.getElementById(parent);

	el.style.visibility = "visible";
	el.style.left = spot.offsetLeft + l.offsetLeft + "px";
	el.style.top = spot.offsetTop + l.offsetTop + "px";

	fcTimeout = setTimeout("document.getElementById('fcPopup').style.visibility = 'hidden'", 3000);
}

function fcUploadValidate() {
	if (document.upload.file.value == "") {
		return false;
	}

	return true;
}
</script>
<form enctype="multipart/form-data" action="$self?page=fcUpload" method="POST" name="upload" onSubmit="return fcUploadValidate()">
<input type="hidden" name="MAX_FILE_SIZE" value="20000000" />
<p>File: <input type="file" name="file" />&nbsp;<input type="submit" value="Upload" />
</form>
<div id="fcPopup"> </div>
<table class="widefat" id="files">
<thead>
	<tr><th>File</th><th>Modified</th><th>Size</th></tr>
</thead>
<tbody>
EOF;
	$fc = get_option('filecache');
	$dh = opendir($fc[fcpath]);

	$elem = 1;
	$fArr = array();
	while (($file = readdir($dh)) !== false) {
		if ($file[0] == ".") {
			continue;
		}

		$fArr[strtoupper(base64_decode($file))] = $file;
	}

	ksort($fArr);
	reset($fArr);
	foreach ($fArr as $file) {
		$p = $fc[fcpath] . "/" . $file;
		$mTime = date("d/m/Y H:s", filemtime($p));
		$size = filesize($p);
		if ($size >= 1048576) {
			$size = (round($size / 1048576)) . "MB";
		} else if ($size >= 1024) {
			$size = (round($size / 1024)) . "KB";
		} else {
			$size .= "B";
		}

		echo "<tr id=\"pop" . $elem . "\"><td><a href=\"javascript:fcPopup('pop" . $elem . "', '" . urlencode($file) . "')\">" . base64_decode($file) . "</td><td>" . $mTime . "</td><td>" . $size . "</td></tr>";
		$elem++;
	}

	echo <<<EOF
</tbody>
</table>
</div>
EOF;
}

function FileCache_upload() {
	$fc = get_option('filecache');

	if ((is_user_logged_in()) &&
	    (isset($_POST['MAX_FILE_SIZE'])) &&
	    (isset($_FILES['file']['tmp_name']))) {
		$dst = $fc[fcpath] . "/" . base64_encode(basename($_FILES['file']['name']));
		move_uploaded_file($_FILES['file']['tmp_name'], $dst);
	}

	FileCache_main();
}

function FileCache_options() {
	echo <<<EOF
<div class="wrap">
<h2>File Cache Options</h2>
<form method="post" action="options.php">
EOF;

	settings_fields('filecache-options-group');

	$fc = get_option('filecache');

	echo <<<EOF
<table class="form-table">
<tr valign="top">
<th scope="row">Filecache path</th>
<td><input type="text" name="filecache[fcpath]" value="$fc[fcpath]" /></td>
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
