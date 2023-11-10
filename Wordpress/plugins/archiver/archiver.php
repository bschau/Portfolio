<?php
/*
 * Plugin Name: Archiver
 * Plugin URI: http://www.pixel44.dk/
 * Description: Archive posts after a certain date.
 * Version: 1.0
 * Author: Brian Schau
 * Author URI: http://www.pixel44.dk/
 */
	if (is_admin()) {
		load_plugin_textdomain('archiver', '/wp-content/plugins/archiver');
		add_action('admin_menu', 'archiver_menu');
		add_action('admin_init', 'archiver_init');
	}

	add_action('edit_form_advanced', 'archiver_add_box');
	add_action('save_post', 'archiver_save_archive_date');
	add_filter('manage_posts_columns', 'archiver_add_column');
	add_action('manage_posts_custom_column', 'archiver_show_date');

function archiver_menu() {
	add_options_page(__('Archiver', 'archiver'), __('Archiver', 'archiver'), 'administrator', 'archiver.php', 'archiver_options');
}

function archiver_init() {
	register_setting('archiver-options-group', 'archiver');
}

function archiver_options() {
	echo "<div class=\"wrap\">";
	echo "<h2>" . __("Archive", 'archiver') . "</h2>";
	echo "<form method=\"post\" action=\"options.php\">";

	settings_fields('archiver-options-group');

	$arc = get_option('archiver');
	$categories = get_categories('hide_empty=0');

	echo <<<EOF
<table class="form-table">
<tr valign="top">
EOF;
	echo "<th scope=\"row\">" . __("Archive Category", 'archiver') . "</th>";

	echo <<<EOF
<td><select name="archiver[category]">
EOF;
	
	foreach ($categories as $cat) {
		echo "<option value=\"" . $cat->cat_ID . "\"";
		if ($arc[category] == $cat->cat_ID) {
			echo " selected>";
		} else {
			echo ">";
		}

		echo $cat->cat_name . "</option>";
	}

	echo <<<EOF
</select></td>
</tr>
</table>
<p class="submit">
<input type="submit" class="button-primary" value="
EOF;
	_e("Save Changes", 'archiver');
	echo <<<EOF
" /></p>
</form>
</div>
EOF;
}

function archiver_add_box() {
	add_meta_box('Archiver', __('Archive', 'archiver'), 'archiver_archive_box', 'post', 'advanced', 'high');
}

function archiver_archive_box() {
	global $wpdb, $post, $wp_locale;

	$id = $post->ID;
	$date = archiver_get_date($id);

	if ($date) {
		list($ay, $am, $ad) = explode('/', $date);
	} else {
		$ay = "";
		$am = "";
		$ad = "";
	}

	echo "<p>";
	_e("Archive", 'archiver');
	echo " <input type=\"text\" id=\"ad\" name=\"ad\" value=\"" . $ad . "\" size=\"2\" maxlength=\"2\"> ";
	echo "<select id=\"am\" name=\"am\"><option value=\"\"";
	if ($am == '') {
		echo " selected";
	}

	echo "></option>";
	for ($m = 1; $m < 13; $m++) {
		echo "<option value=\"" . zeroise($m, 2) . "\"";
		if ($m == $am) {
			echo " selected";
		}

		echo ">" . $wp_locale->get_month($m) . "</option>";
	}

	echo "<input type=\"text\" id=\"ay\" name=\"ay\" value=\"" . $ay . "\" size=\"4\" maxlength=\"4\"> ";
	_e("day month year", 'archiver');
	echo "</p>";

	echo "<p>" . __("The post will not be archived if the above is not set.", 'archiver') . "</p>";
}

function archiver_get_date($id) {
	global $wpdb;

	$sql = "SELECT meta_value FROM " . $wpdb->postmeta . " WHERE meta_key='_archiver_date' AND post_id='" . $id ."'";
	$d = $wpdb->get_var($sql);

	if ($d) {
		return $d;
	}

	return false;
}

function archiver_save_archive_date($id) {
	if (isset($_POST['ay'])) {
		$ay = trim($_POST['ay']);
	} else {
		$ay = false;
	}

	if (isset($_POST['am'])) {
		$am = trim($_POST['am']);
	} else {
		$am = false;
	}

	if (isset($_POST['ad'])) {
		$ad = trim($_POST['ad']);
	} else {
		$ad = false;
	}

	if (($ay == false) && ($am == false) && ($ad == false)) {
		archiver_set_date($id, false);
	} else {
		archiver_set_date($id, $ay . "/" . zeroise($am, 2) . "/" . zeroise($ad, 2));
	}
}

function archiver_set_date($id, $d) {
	global $wpdb;

	$old_arc_date = archiver_get_date($id);
	$where = "WHERE post_id='" . $id . "' AND meta_key='_archiver_date'";
	$sql = false;
	if ($old_arc_date !== false) {
		if ($d) {
			$sql = "UPDATE " . $wpdb->postmeta . " SET meta_value='" . $d . "' " . $where;
		} else {
			$sql = "DELETE FROM " . $wpdb->postmeta . " " . $where;
		}
	} else if ($d !== false) {
		$sql = "INSERT INTO " . $wpdb->postmeta . " (post_id, meta_key, meta_value) VALUES(" . $id . ", '_archiver_date', '" . $d . "')";
	}

	if ($sql !== false) {
		$wpdb->query($sql);
	}
}

function archiver_add_column($columns) {
	$columns['Archiver'] = __("Archive", 'archiver');
	return $columns;
}

function archiver_show_date($column_name) {
	global $wpdb, $post;

	if ($column_name == 'Archiver') {
		if (($d = archiver_get_date($post->ID))) {
			echo $d;
		}
	}
}

?>
