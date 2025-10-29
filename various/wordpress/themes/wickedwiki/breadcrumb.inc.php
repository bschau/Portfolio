<?php
	echo "<p class=\"breadcrumb\"><a href=\"" . $url . "\">Home</a>";

	if (!empty($category)) {
		$trail = "";
		do {
			$link = get_category_link($category->cat_ID);
			$trail = " &rarr; <a href=\"" . $link . "\">" . $category->name . "</a>" . $trail;
			$id = $category->category_parent;
			$category = intval($id) === 0
				? false
				: get_category($id);
		} while ($category !== false);
	}

	if (!empty($post_name)) {
		$trail = $trail . " &rarr; " . $post_name;
	}

	echo $trail . "</p>";
?>
