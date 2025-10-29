<?php
function the_entry($post) {
    $title = trim($post->post_title);
    if (strlen($title) < 1) {
        $title = "(blank)";
    }

    $class = $post->post_type === "page" ? "page_item" : "post-item";
    $link = get_permalink($post->ID);
    $html = "<div class=\"entry\">";
    $html .= "<h4><a href=\"" . $link . "\">" . $title . "</a></h4>";
    $content = wp_trim_words($post->post_content, 30, '');
    $content .= " <a href=\"" . $link . "\">(mere)</a>";
    $html .= "<p>" . $content . "</p></div>";
    return $html;
}

function row_init() {
    global $item;

    $item = 0;
}

function row_header() {
    global $item;

    if ($item == 0) {
        echo "<div class=\"entry-row\">";
    }
    $item++;
}

function row_footer() {
    global $item;

    if ($item < 3) {
        return;
    }

    echo "</div>";
    $item = 0;
}
?>
