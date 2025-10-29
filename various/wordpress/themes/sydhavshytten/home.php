<?php
/*
Template Name: Home Page
*/
	require_once('header.inc.php');
	the_post();
	$image = get_template_directory_uri() . "/sydhavshytten.png";
?>
<!-- home.php -->
<div id="page">
	<p class="center">
		<img src="<?php echo $image; ?>" alt="Sydhavshytten" />
	</p>
	<p><br /></p>
</div>
<?php
	get_footer();
?>
