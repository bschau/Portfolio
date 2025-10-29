<!DOCTYPE html>
<html <?php language_attributes(); ?>>
<head>
	<meta charset="<?php bloginfo('charset'); ?>">
	<title><?php wp_title(); ?></title>
	<meta name="viewport" content="width=device-width">
	<link rel="profile" href="http://gmpg.org/xfn/11" />
	<link rel="stylesheet" type="text/css" media="all" href="<?php bloginfo('stylesheet_url'); ?>" />
	<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon" />
	<link rel="icon" href="/favicon.ico" type="image/x-icon" />
<?php
	wp_head();
	$url = get_site_url();
?>
</head>
<body <?php body_class(); ?>>
	<div class="logo center"><a href="/" class="logo-link">Sydhavshytten</a></div>
	<div class="header-menu">
		<a href="/index.php/category/eateries/">Spisesteder</a> |
		<a href="/index.php/category/sightseeing/">Sev&aelig;rdigheder</a> |
		<a href="/index.php/praktisk-info/">Praktisk info</a>
		<form action="<?php bloginfo('url'); ?>" method="get" id="searchform">
		    <input type="text" class="search" placeholder="S&oslash;g ..." name="s" id="search-text" />
			<a onclick="javascript:document.getElementById('searchform').submit();" class="icon-button" id="search-submit">&#x1F50D;</a>
		</form>
	</div>
	<p style="clear: both"><br /></p>
