<?php get_header(); ?>
		<div id="content">
<?php
	if (have_posts()) {
		while (have_posts()) {
			the_post();
?>
				<div id="post-<?php the_ID(); ?>" <?php post_class(); ?>>
					<h1 class="entry-title"><?php the_title(); ?></h1>
					<hr />
					<?php the_content(); ?>
				</div>
<?php
		}
	}
?>
		</div>
<?php get_footer(); ?>
