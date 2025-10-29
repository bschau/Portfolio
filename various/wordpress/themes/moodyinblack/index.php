<?php get_header(); ?>
<?php if (!have_posts()) { ?>
		<div class="notepad"><a href="<?php bloginfo('wpurl'); ?>/"><img src="<?php bloginfo('stylesheet_directory'); ?>/notepad.png" width="75" height="100" alt="Notepad" /></a></div>

			<div class="post">
				<h1 class="entry-title">Nothing here ...</h1>
				<p>Nothing found using your search terms ...</p>
			</div>
<?php
	} else {
		$esep = "";
		while (have_posts()) {
			the_post();

			echo $esep;
			$esep = "<p class=\"break\"><br /></p>";
?>

			<div class="notepad"><a href="<?php bloginfo('wpurl'); ?>/"><img src="<?php bloginfo('stylesheet_directory'); ?>/notepad.png" width="75" height="100" alt="Notepad" /></a></div>
			<div class="post">
				<h2 class="entry-title"><a href="<?php the_permalink(); ?>"><?php the_title(); ?></a></h2>

				<?php the_content(); ?>
			</div>
<?php
		}
	}
?>
<?php get_footer(); ?>
