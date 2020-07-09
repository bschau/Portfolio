<?php
/*
Template Name: Single Post
*/
	require_once('header.inc.php');
	require_once('entry.inc.php');
	the_post();
	$category = get_the_category()[0];
?>
<!-- single.php -->
	<div id="page">
		<div class="page-content">
			<h1><?php echo get_the_title(); ?></h1>
<?php the_content(); ?>
		</div>
		<p><br /></p>
		<p class="post-in-category-title">Gemt i: <i><?php echo $category->name; ?></i></p>
	</div>
<?php
	get_footer();
?>
