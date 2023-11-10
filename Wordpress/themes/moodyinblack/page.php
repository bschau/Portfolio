<?php get_header(); ?>
<?php
	if (have_posts()) {
		while (have_posts()) {
			the_post();
?>
			<div class="notepad"><a href="<?php bloginfo('wpurl'); ?>/"><img src="<?php bloginfo('stylesheet_directory'); ?>/notepad.png" width="75" height="100" alt="Hjelm" /></a></div>
			<div class="post">
				<h2 class="entry-title"><a href="<?php the_permalink(); ?>"><?php the_title(); ?></a></h2>

				<?php the_content(); ?>
			</div>
<?php
		}
	}
?>
<?php get_footer(); ?>
