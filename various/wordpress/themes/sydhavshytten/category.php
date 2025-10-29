<?php
/*
Template Name: Category
*/
function get_description($category) {
	if (!isset($category->description)) {
		return "";
	}

	return trim($category->description);
}
	require_once('header.inc.php');
	require_once('entry.inc.php');
	$category = get_queried_object();
	$description = get_description($category);
	$category_id = $wp_query->get_queried_object_id();
?>
<div id="page">
	<div class="page-content">
		<h1><?php echo $category->name; ?></h1>
<?php
	if (strlen($description) > 0) {
		echo "<p>" . $description . "</p><p><br /></p>";
	}

	$args = array(
		"numberposts" => -1,
		"orderby" => "post_title",
		"order" => "asc",
		"category" => $category_id
	);
	$posts = get_posts($args);
	row_init();
	foreach ($posts as $post) {
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
