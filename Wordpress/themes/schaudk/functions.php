<?php
add_action('after_setup_theme', 'schaudk_setup');

if (!function_exists('schaudk_setup')) {
	function schaudk_setup() {
		// This theme uses wp_nav_menu() in one location.
		register_nav_menus(array(
			'primary' => __('Primary Navigation', 'schaudk'),
		));
	}
}
?>
