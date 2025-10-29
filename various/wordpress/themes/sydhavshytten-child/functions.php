<?php
add_action('wp_enqueue_scripts', 'enqueue_parent_styles');

function enqueue_parent_styles() {
	wp_enqueue_style('sydhavshytten-style', get_stylesheet_uri(), array('friendly-lite-style'));
}

function friendly_lite_breadcrumb() {
}

?>
