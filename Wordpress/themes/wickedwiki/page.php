<?php
/*
Template Name: Single Page
*/
	require_once('header.inc.php');
	the_post();
?>
<!-- page.php -->
<body <?php body_class(); ?>>
<div id="page">
<?php
	require_once('searchbox.inc.php');
	$category = get_the_category();
        $category = $category[0];
        $post_name = get_the_title();
?>
<div class="page-content">
<?php
	require_once('breadcrumb.inc.php');
?>
	<h1 class="wwh1"><?php echo $post_name; ?></h1>
	<?php the_content(); ?>
</div>
<p><br /></p>
</div>
<?php
	get_footer();
?>
