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
	require_once('breadcrumb.inc.php');
?>
	<h1 class="wwh1">Sections</h1>
	<div class="lists">
		<ul>
<?php
	wp_list_categories('show_count=1&hide_empty=1&title_li=');
?>
		</ul>
	</div>
	<div class="lists">
		<ul>
<?php
	wp_list_pages('title_li=');
?>
		</ul>
	</div>
</div>
<p><br /></p>
</div>
<?php
	get_footer();
?>
