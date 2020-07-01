<?php
	add_action('after_setup_theme', 'sjs_setup');

	if (!function_exists('sjs_setup'))
	{
function sjs_setup()
{
	register_nav_menus(array(
		'primary' => __('Primary Navigation', 'main'),
	));
}
	}

function sjs_footer()
{
	echo <<<EOF
<div id="footer">
	<p>Sj&aelig;lsmarkvej 3, 2970 H&oslash;rsholm :: +45 2729 0525 :: sjs-skovboernehave@hotmail.dk</p>
</div>
EOF;
}
?>
