<!DOCTYPE html>
<html dir="ltr" <?php language_attributes(); ?>>
<head>
	<meta name="viewport" content="width=device-width">
	<meta charset="<?php bloginfo('charset'); ?>" />
	<title><?php wp_title(); ?></title>
	<link rel="shortcut icon" href="<?php echo get_template_directory_uri(); ?>/favicon.png" type="image/x-icon" />
	<link rel="icon" href="<?php echo get_template_directory_uri(); ?>/favicon.png" type="image/x-icon" />
	<link rel="profile" href="http://gmpg.org/xfn/11" />
	<link rel="stylesheet" type="text/css" media="all" href="<?php bloginfo( 'stylesheet_url' ); ?>" />
	<script src="<?php echo get_template_directory_uri(); ?>/js.js" type="text/javascript"></script>
	<?php wp_head(); ?>
</head>
<body <?php body_class(); ?> onload="initLdx()">
