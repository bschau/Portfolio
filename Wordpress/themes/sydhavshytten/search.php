<?php
/*
Template Name: Search Page
*/
	require_once('header.inc.php');
	require_once('entry.inc.php');
?>
	<div id="page">
		<div class="page-content">
			<h1>S&oslash;geresultat</h1>
<?php
	row_init();
	while (have_posts()) {
		the_post();
		row_header();
		echo the_entry($post);
		row_footer();
	}
?>
		</div>
		<p><br /></p>
	</div>
<?php
	get_footer();
?>
