<?php
	require_once('../../../wp-config.php');
	require_once('../../../wp-includes/wp-db.php');

	$arc = get_option('archiver');
	$arc_cat = $arc[category];

	$wpdb = new wpdb(DB_USER, DB_PASSWORD, DB_NAME, DB_HOST);
	$wpdb->set_prefix($table_prefix);

	$today = date('Y/m/d');
	$sql = "SELECT post_id,meta_value FROM " . $wpdb->postmeta . " WHERE meta_key='_archiver_date' AND meta_value<'" . $today . "'";
	$posts = $wpdb->get_results($sql);

	foreach ($posts as $p) {
		wp_update_post(array('ID' => $p->post_id, 'post_category'=>array($arc_cat), 'post_status'=>'draft','meta_value'=>$p->meta_value));
	}
?>
