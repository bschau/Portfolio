<?php
function day_color($day) {
	$h = md5($day);

	return '#' . $h{0} . $h{5} . $h{11} . $h{16} . $h{21} . $h{26};
}
?>
