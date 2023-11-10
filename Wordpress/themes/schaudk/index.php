<?php get_header(); ?>
	<div id="content" role="main">
<?php
	$shown = 0;	
	if (have_posts()) {
		if ($wp_query->max_num_pages > 1) { ?>
			<div id="nav-below" class="navigation">
				<div class="nav-previous"><?php previous_posts_link('<span class="meta-nav">&larr;</span> Nyere'); ?></div>
				<div class="nav-next"><?php next_posts_link('&AElig;ldre <span class="meta-nav">&rarr;</span>'); ?></div>
			</div>
<?php
		} 

		$emitSep = "";
		while (have_posts()) {
			the_post();

			if (is_home()) {
				$c = get_the_category();

				if (strcmp($c[0]->cat_name, "Opskrifter") == 0) {
					continue;
				}
			}

			$shown++;
			echo $emitSep;
			$emitSep = "<hr />";
?>

			<div id="post-<?php the_ID(); ?>" <?php post_class(); ?>>
				<h2 class="entry-title"><a href="<?php the_permalink(); ?>"><?php the_title(); ?></a></h2>

				<?php the_content(); ?>
			</div>
<?php
		}
	} else if (isset($_GET['s'])) {
		$shown = 1;
?>
		<div id="post-0" class="post error404 not-found">
			<h1 class="entry-title">Ingen poster</h1>
			<p>Hmmm ... der blev ikke fundet nogle poster der matchede dit s&oslash;gekriterie ...</p>
		</div>
<?php
	}

	if ($shown == 0) { ?>
		<div id="post-0" class="post error404 not-found">
			<h1 class="entry-title">Passer gaden!</h1>
			<p>Der er intet at se her ... m&aring;ske en dag ...?</p>
		</div>
<?php
	}
?>
	</div>
<?php get_footer(); ?>
