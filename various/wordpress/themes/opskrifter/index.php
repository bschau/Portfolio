<?php
/*
Template Name: Index
*/
	require_once('header.inc.php');
?>
<body <?php body_class(); ?>>
<div id="page">
<?php
	require_once('searchbox.inc.php');
	$category = false;
?>
<div class="page-content">
<?php
	$categories = get_categories();
	foreach ($categories as $category) {
		$args = array('numberposts' => -1, 'category' => $category->cat_ID, 'orderby' => 'title', 'order' => 'ASC');
		$posts = get_posts($args);
		if (count($posts) < 1) {
			continue;
		}

		reset($posts);
		echo "<h2>" . $category->cat_name . "</h2><p>";
		foreach ($posts as $post) {
			echo "<a href=\"" . get_permalink($post->ID) . "\" class=\"cat-item\">" . $post->post_title . "</a>";
		}
		echo "</p><p style=\"clear: both\"><br /></p>";
	}


	/*
	print_r($posts);
	 */

	echo "<p><br /></p>";
//	print_r(get_categories());
?>
</div>
<p><br /></p>
</div>
<?php
	get_footer();
?>
