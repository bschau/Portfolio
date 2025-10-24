<?php
/*
Template Name: Index
*/
	require_once('header.php');
?>
<body <?php body_class(); ?>>
<div id="page">
<?php
	$category = false;
?>
<div class="page-content">
	<h1>Sections</h1>
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
</div>
<?php
	get_footer();
?>
