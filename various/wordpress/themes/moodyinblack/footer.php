		</div> <!-- main -->

		<div id="sidebar">
		<form role="search" method="get" id="searchform" action="<?php bloginfo('wpurl'); ?>/" >
			<p>
                        <input type="text" value="" name="s" id="s" />&nbsp;<input type="submit" id="searchsubmit" value="Search" />
			</p>
                </form>

<ul>
	<li class="page_item"><a href="<?php bloginfo('wpurl'); ?>/" title="Home">Home</a></li>
	<li class="page_item"><a href="<?php bloginfo('wpurl'); ?>/wp-admin/" title="Login">Login</a></li>
</ul>
<ul><?php
	wp_list_pages(array('title_li' => ''));
?></ul>
<ul><?php
	wp_list_categories(array('title_li' => ''));
?></ul>
<p><br /></p>
	<script src="http://pagead2.googlesyndication.com/pagead/show_ads.js"></script>
		</div> <!-- sidebar -->
	</div> <!-- wrap -->
	<p class="break"><br /></p>
	</div> <!-- superwrap -->

<?php
	$dc = day_color(date('z'));
?>
	<div style="height: 10px; clear: both; background-color: <?php echo $dc; ?>"></div>

	<div id="footer">
		<p id="navbuttons">
<?php
	posts_nav_link('&nbsp;&nbsp;&nbsp;&nbsp;', '&lt;&lt;', '&gt;&gt;');
?>
		</p>
		<p><br /></p>
		<p class="small italic">Theme: Moody in Black &copy; Brian Schau, 2011</p>
	</div>
</div> <!-- wrapper -->
<?php wp_footer(); ?>
</body>
</html>
