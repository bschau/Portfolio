<?php
/*
Template Name: Single Page
*/
	require_once('header.inc.php');
	require_once('entry.inc.php');
	the_post();
?>
<!-- page.php -->
	<div id="page">
		<div class="page-content">
			<h1><?php echo get_the_title(); ?></h1>
<?php the_content(); ?>
		</div>
		<p><br /></p>
	</div>
<?php
	get_footer();
?>