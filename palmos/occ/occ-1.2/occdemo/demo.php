<HTML>
<HEAD>
<TITLE>Demonstration</TITLE>
<META NAME="PalmComputingPlatform" CONTENT="true">
</HEAD>
<BODY>
<P>Congratulations!
<P>Your One Click Clicking Works!
<P>Invoked by a
<?php 
	if (isset($type)) {
		if (strcmp($type, "norm"))
			echo "standalone";
		else
			echo "normal";
	} else
		echo "unknown";

	echo " OCC.";
?>
</BODY>
</HTML>
