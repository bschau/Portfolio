<!DOCTYPE html>
<html <?php language_attributes(); ?>>
<head>
	<meta charset="<?php bloginfo('charset'); ?>" />
	<title><?php wp_title(); ?></title>
	<link rel="profile" href="http://gmpg.org/xfn/11" />
	<link rel="stylesheet" type="text/css" media="all" href="<?php bloginfo('stylesheet_url'); ?>" />
	<link rel="shortcut icon" href="/ww.ico" type="image/x-icon" />
	<link rel="icon" href="/ww.ico" type="image/x-icon" />
	<meta name="viewport" content="width=device-width">
<?php
	wp_head();
	$url = get_site_url();
?>
</head>
