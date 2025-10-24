<?php
/*
Plugin Name: Gallery By ID
Plugin URI: http://www.pixel44.dk/
Description: Create a gallery by ID using the [GALLERY] short tag. It adds two attributes to the [GALLERY] short tag - 'ids' and 'css'. 'ids' is a list of Menu Order ids to include in the gallery, such as "1,2,3,8,9,11,14'. 'css' can be set to 'full', 'size' or 'none' and controls the amount of style sheet output. Setting 'css' to 'full' gives full output. Setting 'css' to 'size' only outputs the width of each image. Setting 'css' to 'none' gives no style sheet output.
Author: Brian Schau
Version: 1.0
Author URI: http://www.pixel44.dk/
*/
remove_shortcode('gallery');

add_shortcode('gallery', 'gallery_by_id');

/**
 * This function looks very much like the built in gallery_shortcode function.
 * It is in fact the same ... but with a few tweaks :-)
 */
function gallery_by_id($attr) {
	global $post, $wpdb;

	if (($output = apply_filters('post_gallery', '', $attr)) != '') {
		return $output;
	}

	if (isset($attr['orderby'] ) ) {
		$attr['orderby'] = sanitize_sql_orderby($attr['orderby']);
		if (!$attr['orderby']) {
			unset($attr['orderby']);
		}
	}

	extract(shortcode_atts(array(
		'order'		=> 'ASC',
		'orderby'	=> 'menu_order,ID',
		'id'		=> $post->ID,
		'itemtag'	=> 'dl',
		'icontag'	=> 'dt',
		'captiontag'	=> 'dd',
		'columns'	=> 3,
		'size'		=> 'thumbnail',
		'css'		=> 'full',
		'ids'		=> '',
		), $attr));

	$id = intval($id);
	$q = "SELECT * FROM " . $wpdb->posts . " WHERE post_parent=" . $id . " AND post_status='inherit' AND post_type='attachment' AND post_mime_type LIKE 'image/%'";
	if (!empty($ids)) {
		$q .= " AND menu_order IN (" . $ids . ")";
	}
	$q .= " ORDER BY " . $orderby . " " . $order;

	$attachments = $wpdb->get_results($q);

	if (empty($attachments)) {
		return $output;
	}

	$a = array();
	if (!empty($ids)) {
		$objs = array();
		foreach ($attachments as $att) {
			$objs[$att->menu_order] = $att->ID;
		}

		$i = explode("," , $ids);
		while (list($k, $v) = each($i)) {
			$a[] .= wp_get_attachment_link($objs[$v], $size, true);
		}
	} else {
		foreach ($attachments as $att) {
			$a[] .= wp_get_attachment_link($att->ID, $size, true);
		}
	}

	$attachments = $a;

	if (is_feed()) {
		$output .= "\n";
		while (list($k, $v) = each($attachments)) {
			$output .= $v . "\n";
		}

		return $output;
	}

	$listtag = tag_escape($listtag);
	$itemtag = tag_escape($itemtag);
	$captiontag = tag_escape($captiontag);
	$columns = intval($columns);
	$itemwidth = $columns > 0 ? floor(100 / $columns) : 100;
	
	switch ($css) {
	case 'none':
		break;
	case 'size':
		$output = apply_filters('gallery_style', "
		<style type='text/css'>
			.gallery-item {
				width: {$itemwidth}%;
			}
		</style>");
		break;
	default:
		$output .= apply_filters('gallery_style', "
		<style type='text/css'>
			.gallery {
				margin: auto;
			}
			.gallery-item {
				float: left;
				margin-top: 10px;
				text-align: center;
				width: {$itemwidth}%;
			}
			.gallery img {
				border: 2px solid gray;
			}
			.gallery-caption {
				margin-left: 0;
			}
		</style>");
		break;
	}

	$output .= "<div id='gallery'>";
	$i = 0;
	while (list($k, $v) = each($attachments)) {
		$output .= "<{$itemtag} class='gallery-item'>";
		$output .= "
			<{$icontag} class='gallery-icon'>
				$v
			</{$icontag}>";
		if (($captiontag) && (trim($attachment->post_excerpt))) {
			$output .= "
				<{$captiontag} class='gallery-caption'>
				{$attachment->post_excerpt}
				</{$captiontag}>";
		}
		$output .= "</{$itemtag}>";
		if (($columns > 0) && ((++$i) % $columns == 0)) {
			$output .= '<br style="clear: both" />';
		}
	}

	$output .= "
			<br style='clear: both;' />
		</div>\n";

	return $output;
}

?>
