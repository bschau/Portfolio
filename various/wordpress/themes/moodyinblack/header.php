<!DOCTYPE html>
<html <?php language_attributes(); ?>>
<head>
	<meta charset="<?php bloginfo( 'charset' ); ?>" />
	<title><?php wp_title('|'); ?></title>
	<link rel="profile" href="http://gmpg.org/xfn/11" />
	<link rel="stylesheet" type="text/css" media="all" href="<?php bloginfo( 'stylesheet_url' ); ?>" />
	<script>
google_ad_client = "ca-pub-8346512567628995";
google_ad_slot = "8147260675";
google_ad_width = 120;
google_ad_height = 600;
	</script>
<?php
	wp_head();

	$dc = day_color(date('z'));
?>
<script type="text/javascript">
var _gaq = _gaq || [];
_gaq.push(['_setAccount', 'UA-24107836-2']);
_gaq.push(['_trackPageview']);

(function() {
	var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
	ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
	var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
})();
</script>
</head>

<body <?php body_class(); ?>>
<div id="wrapper">
	<div id="header">
		<p><br /></p>
		<p><br /></p>
		<p><br /></p>
		<h1 class="center">Family Funch Schaus Website</h1>
		<p><br /></p>
		<p><br /></p>
		<p class="small">... some day we might write something interesting here ...</p>
	</div>
	<div style="height: 10px; background-color: <?php echo $dc; ?>"></div>

	<div id="superwrap">
	<div id="wrap">
		<div id="main">
