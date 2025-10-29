<?php
/*
Template Name: Search Page
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
		<h1 class="wwh1">S&oslash;geresultat</h1>
		<div class="lists">
			<ul>
<?php
	while (have_posts()) {
		the_post();
		$title = trim($post->post_title);
		if (strlen($title) < 1) {
			$title = "(blank)";
		}

		echo "<li><a href=\"" . get_permalink($post->ID) . "\" class=\"cat-item\">" . $title . "</a></li>";
	}
?>
			</ul>
		</div>
	</div>
<p><br /></p>
<?php
	get_footer();
?>
</div>
