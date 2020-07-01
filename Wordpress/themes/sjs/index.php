<?php
/*
Template Name: Sider
*/
?>
<!DOCTYPE html>
<html <?php language_attributes(); ?>>
<head>
	<meta charset="<?php bloginfo('charset'); ?>" />
	<title><?php wp_title(); ?></title>
	<link rel="profile" href="http://gmpg.org/xfn/11" />
	<link rel="stylesheet" type="text/css" media="all" href="<?php bloginfo('stylesheet_url'); ?>" />
<!--[if !IE 7]>
	<style type="text/xss">
#page
{
	display: table;
	height: 100%;
}
<![endif]-->
	<?php wp_head(); ?>
</head>

<body <?php body_class(); ?>>
<div id="page">
<?php
	require_once('inc/topbar.inc.php');
	the_post();
?>
	<p style="clear: both;">&nbsp;</p>
	<div id="content">
		<div class="box">
<?php
	the_content();
?>
		</div>
		<p><br /></p>
	</div>
</div>
<?php
	get_footer();
?>
