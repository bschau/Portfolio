<?php
/*
Template Name: Category
*/
	require_once('header.inc.php');
	$tab_selected = "wiki";
?>
<body <?php body_class(); ?>>
<div id="page">
<?php
	require_once('searchbox.inc.php');
	$category = get_queried_object();
	$post_name = false;
?>
<div class="page-content">
	<h1 class="wwh1">Opskrifter</h1>
	<div class="lists">
		<ul>
<?php
	$category_id = $wp_query->get_queried_object_id();
	$args = array(
		"show_count" => 1,
		"hide_empty" => 0,
		"title_li" => "",
		"show_option_none" => "",
		"child_of" => $category_id
	);
	wp_list_categories($args);
?>
		</ul>
	</div>
	<div class="lists">
		<ul>
<?php
	$args = array(
		"numberposts" => -1,
		"orderby" => "post_title",
		"order" => "asc",
		"category" => $category_id
	);
	$posts = get_posts($args);
	foreach ($posts as $post) {
		$title = trim($post->post_title);
		if (strlen($title) < 1) {
			$title = "(blank)";
		}

		echo "<li class=\"post-item\"><a href=\"" . get_permalink($post->ID) . "\">" . $title . "</a></li>\n";
	}
?>
		</ul>
	</div>
</div>
<p><br /></p>
</div>
<?php
	get_footer();
?>
