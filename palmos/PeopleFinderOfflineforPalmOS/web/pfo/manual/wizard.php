<?php
	$pages=array("0" => "Unknown page", "1" => "The Welcome Screen", "2" => "Data Collection", "3" => "Working ...", "4" => "Conversion Status");

	if (isset($_GET['p'])==true) {
		$p=trim($_GET['p']);
		if (strlen($p)==0)
			$p="0";
	} else
		$p="0";
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<HTML>
<HEAD><TITLE><?php echo $pages[$p] ?></TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=ISO-8859-1">
<LINK REL="STYLESHEET" TYPE="text/css" HREF="/hp/stylesheet.css">
</HEAD>
<BODY>
<?php
	if ($p=="0")
		echo "<P>Huh? Unknown page ...";
	else
		echo "<TABLE><TR><TD STYLE=\"vertical-align: middle; text-align: center;\"><IMG SRC=\"gfx/wizard".$p."l.jpg\" WIDTH=\"544\" HEIGHT=\"416\" ALT=\"".$pages[$p]."\"></TD></TR></TABLE>";
?>		
</BODY>
</HTML>
