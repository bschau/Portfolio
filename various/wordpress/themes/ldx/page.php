<?php
/*
Template Name: Single Page
*/
	require_once('header.php');
	the_post();
?>
<!-- page.php -->
<body <?php body_class(); ?>>
<div id="page">
<?php
	$category = get_the_category();
        $category = $category[0];
        $post_name = get_the_title();
?>
<div class="page-content">
	<h1><?php echo $post_name; ?></h1>
	<?php the_content(); ?>
</div>
</div>
<?php
	get_footer();
?>
