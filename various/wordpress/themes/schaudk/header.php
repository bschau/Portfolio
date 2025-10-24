<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html <?php language_attributes(); ?>>
<head>
	<meta charset="<?php bloginfo( 'charset' ); ?>" />
	<title><?php wp_title(' '); ?></title>
	<link rel="profile" href="http://gmpg.org/xfn/11" />
	<link rel="stylesheet" type="text/css" media="all" href="<?php bloginfo( 'stylesheet_url' ); ?>" />
	<?php wp_head(); ?>
</head>

<body <?php body_class(); ?>>
<div id="wrapper">
	<div id="header">
		<div id="logo">
<?php
	mt_srand();
	$idx = mt_rand() % 2;
	$t = array(0 => "The gate is open - come in!", 1 => "The beach ...");
	$logo = "logo-" . $idx . ".jpg";
	$text = $t[$idx];
?>
	<img src="<?php bloginfo('stylesheet_directory'); ?>/<?php echo $logo; ?>" width="940" height="200" alt="<?php echo $text; ?>" />
		</div>

		<div id="access">
			<?php wp_nav_menu(); ?>
			<div style="float: right; margin: 8px 10px auto 0;">
				<div style="float: left">
				<a href="/wordpress/wp-admin/"><img src="<?php bloginfo('stylesheet_directory'); ?>/login.png" width="24" height="24" alt="Login" style="float: left" /></a>&nbsp;&nbsp;
				</div>

				<div style="float: right">
				<form role="search" method="get" id="searchform" action="/wordpress/" >
					<input type="text" value="" name="s" id="s" />
					<input type="submit" id="searchsubmit" value="S&oslash;g" />
				</form>
				</div>
			</div>
		</div>
	</div>

	<div id="main">
